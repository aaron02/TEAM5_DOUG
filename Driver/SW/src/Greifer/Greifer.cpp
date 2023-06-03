/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "Greifer.h"

Greifer::Greifer(uint8_t servoPin1, uint8_t servoPin2, PosAntrieb& dreh, Navigation& nav) : mAntrieb(&dreh), mNavigation(&nav)
{
    servo[SERVO_BASE].attach(servoPin1);
    servo[SERVO_GRIPP].attach(servoPin2);

    servo[SERVO_BASE].write(BASE_GS);
    servo[SERVO_GRIPP].write(GRIPP_OFFEN);

    servoPos[SERVO_BASE] = BASE_GS;
    setSollPosition(SERVO_BASE, BASE_GS);
    servoPos[SERVO_GRIPP] = GRIPP_OFFEN;
    setSollPosition(SERVO_GRIPP, GRIPP_OFFEN);
}

Greifer::~Greifer()
{
    servo[SERVO_BASE].detach();
    servo[SERVO_GRIPP].detach();
}

void Greifer::Update(uint64_t difftime)
{
    switch (getArmStatus())
    {
        case ArmStatus::AS_Undefined:
        {
            Grundstellung();
        } break;
        case ArmStatus::AS_PickPackage:
        {
            PickPackage();
        } break;
        case ArmStatus::AS_PlacePackage:
        {
            PlacePackage();
        } break;
        default:
            break;
    }

    // Servo 1
    runServo(SERVO_BASE, difftime);

    // Servo 2
    runServo(SERVO_GRIPP, difftime);

}

//Prüft ob alle Aktoren die gewünschte Position erreicht
bool Greifer::inposition()
{
    //bool inPos = (getPosition(SERVO_BASE) == getSollPosition(SERVO_BASE)) && (getPosition(SERVO_GRIPP) == getSollPosition(SERVO_GRIPP)) && (mAntrieb->inPosition());
    
    int8_t diff1 = (getPosition(SERVO_BASE) - getSollPosition(SERVO_BASE));
    int8_t diff2 = (getPosition(SERVO_GRIPP) - getSollPosition(SERVO_GRIPP));
    bool inPos = diff1 <= 1 && diff1 >= -1;
    bool inPos1 = diff2 <= 1 && diff2 >= -1;

    return inPos && inPos1 && (mAntrieb->inPosition());
}

void Greifer::runServo(ServoMapping servoIndex, uint64_t difftime)
{
    //if (getPosition(servoIndex) != getSollPosition(servoIndex))
    {
        if (updateTimer[servoIndex] <= 0)
        {
            setTimer(servoIndex, 100 * TimeVar::Millis); //1000:500 -> 2 grad/s

            int8_t value = (getPosition(servoIndex) < getSollPosition(servoIndex)) ? 1 : -1;

            // Servo Um 1 Grad Bewegen
            //sLogger.debug("Servo Nr. %u Istwert Servo %u || Sollwert an Servo %u", servoIndex, getPosition(servoIndex), getSollPosition(servoIndex));
            servo[servoIndex].write(getPosition(servoIndex) + value);
            servoPos[servoIndex] = getPosition(servoIndex) + value;
        }
        else
            updateTimer[servoIndex] -= difftime;
    }
}

uint8_t Greifer::getPosition(ServoMapping servoNumber)
{
    uint8_t mPosition = 0;

    switch (servoNumber)
    {
        case SERVO_BASE:
        {
            mPosition = servoPos[SERVO_BASE]/*map(servo[SERVO_BASE].read(), 0, 1023, 0, 179)*/;
        } break;
        case SERVO_GRIPP:
        {
            mPosition = servoPos[SERVO_GRIPP]/*map(servo[SERVO_GRIPP].read(), 0, 1023, 0, 179)*/;
        } break;
        default:
            sLogger.failure("Servo Unbekannt %u", servo);
            break;
    }

    return mPosition;
}

uint8_t Greifer::getSollPosition(ServoMapping servo)
{
    uint8_t mPosition = 0;

    switch (servo)
    {
        case SERVO_BASE:
        {
            mPosition = servo1PositionToGo;
        } break;
        case SERVO_GRIPP:
        {
            mPosition = servo2PositionToGo;
        } break;
        default:
            sLogger.failure("Servo Unbekannt %u", servo);
            break;
    }

    return mPosition;
}

void Greifer::setSollPosition(ServoMapping servo, SERVO_Positionen degree) 
{
    switch (servo)
    {
        case SERVO_BASE:
        {
            servo1PositionToGo = degree;
        } break;
        case SERVO_GRIPP:
        {
            servo2PositionToGo = degree;
        } break;
        default:
            sLogger.failure("Servo Unbekannt %u", servo);
            break;
    }
}

void Greifer::setTimer(ServoMapping servo, int32_t timer)
{
    if (updateTimer.size() < servo)
    {
        sLogger.failure("Greifer:: SetTimer Failed updateTimer Size = %u and ServoMapping had Index %u", updateTimer.size(), servo);
        return;
    }

     updateTimer[servo] = timer;
}

/*
Grundstellungsfahrt von dem Greifer, Arm und referenzierung des Drehtisches. Soll Kollisionen vermeiden.
Eine Parkposition wird immer am ende angefahren.
Drehtisch: Wird nur referenziert wenn er noch nicht referenziert ist
*/
void Greifer::Grundstellung()
{
    switch (iGrundstellungStep)
    {
        case 0:
        {
            /*
            Base und Gripp werden in einen Sicheren Bereich gefahren. Weiterschaltung nur über Base Zeilposition
            weil nur er eine Kollision erfahren kann. IF-Bedingung soll den nächsten Case Starten wenn Position erreicht ist.
            */
            setSollPosition(SERVO_BASE, BASE_GS);
            setSollPosition(SERVO_GRIPP, GRIPP_OFFEN);

            //sLogger.debug("Servo 1 = %u Servo 2 = %u", getPosition(SERVO_BASE), getPosition(SERVO_GRIPP));

            //Weiterschaltbedingung
            if (getPosition(SERVO_BASE) == getSollPosition(SERVO_BASE))
                iGrundstellungStep++;
        } break;
        case 1:
        {
            // Drehtisch wird referenziert. Nächster Schritt wenn Done.
            if(!mAntrieb->isHomed())
                mAntrieb->setHoming();
            else
                iGrundstellungStep++;
        } break;
        case 2:
        {
            // Drehtisch und SERVO_BASE gehen in die Parkposition.
            mAntrieb->moveAbsolutAngle(DT_Parkposition);
            setSollPosition(SERVO_BASE, BASE_PARKPOSITION);

            //Weiterschaltbedingung
            if ((mAntrieb->inPosition()) && (getPosition(SERVO_BASE) == getSollPosition(SERVO_BASE)))
                iGrundstellungStep++;
        } break;
        case 3:
        {
            // Grundstellung abgeschlossen. Rücksetzten von der INIT variable
            setArmStatus(ArmStatus::AS_Ready);
            setPackStatus(PackStatus::STATUS_OK);
            iGrundstellungStep = 0;
        } break;
    }
}

void Greifer::PickPackage()
{
    switch (iPickPackageStep)
    {
        case 0: // Arm Rauf
        {
            //Arm fährt hoch und Greifer wird geöffnet.
            setSollPosition(SERVO_GRIPP, GRIPP_OFFEN);
            setSollPosition(SERVO_BASE, BASE_OBEN);
            //Weiterschaltbedingung
            if (inposition())
                iPickPackageStep++;
        }
        break; 
        case 1: // Drehen
        {   
            //Arm wird in die Position zum aufnehmen eines Paketes bewegt.
            mAntrieb->moveAbsolutAngle(DT_Greifposition);
            if (inposition())
                iPickPackageStep++;
        }
        break;
        case 2: // Arm Runter
        {   
            //Servo BASE/Arm bewegt sich in die Greifposition.
            setSollPosition(SERVO_BASE, BASE_GEIFERPOSITION);
            if (inposition())
                iPickPackageStep++;
        }
        break;
        case 3: //vorfahren
        {
            setPackStatus(PackStatus::STATUS_ReadyToMove);

            //Roboter fährt zum Paket um es mit dem Greifer aufnehmen zu können #hier wird noch ein Abstandssensor eingebaut
            //mNavigation->setSollPosition(mNavigation->getPosition()->getX(), mNavigation->getPosition()->getY() + 4);
            if (mNavigation->getDrivingState() == DrivingState::DRIVE_STATE_FINISHED)
                iPickPackageStep++;
        }
        break;
        case 4: // Greifen
        {   
            setPackStatus(PackStatus::STATUS_Undefined);
            //Servo Greift das Paket
            setSollPosition(SERVO_GRIPP, GRIPP_GESCHLOSSEN);
            if (inposition())
                iPickPackageStep++;
        }
        break;
        case 5: // zurück fahren
        {   
            setPackStatus(PackStatus::STATUS_ReadyToMove);
            //Roboter fährt zurück damit der Arm wieder sich frei bewegen kann.
            //mNavigation->setSollPosition(mNavigation->getPosition()->getX(), mNavigation->getPosition()->getY() - 4);
            if (mNavigation->getDrivingState() == DrivingState::DRIVE_STATE_FINISHED)
                iPickPackageStep++;
        }
        break;
        case 6: // Arm Hoch
        {   
            setPackStatus(PackStatus::STATUS_Undefined);
            //BASE fährt in die obere Position
            setSollPosition(SERVO_BASE, BASE_OBEN);
            if (inposition())
                iPickPackageStep++;
            
        }
        break;
        case 7: // Drehen mit Freigabe (Optional)
        {   
            //Arm wird auf die Freigabe Position bewegt. Von dort aus kann dem Greifer nix passieren und der Roboter kann zur nächsten aufgabe fahren.
            mAntrieb->moveAbsolutAngle(DT_Bewegung_Freigegeben);
            if (inposition())
                iPickPackageStep++;
        }
        break;
        case 8: // Drehen auf Lagerposition BASE auf HoverOverLager Position
        {   
            //Arm wird auf die gewünschte Lagerposition gedreht
            //Variable iLagerindex gibt die gewünschte Lagerposition an. Mittels getPositionFromIndex wird der Winkel der Lagerposition übertragen.
            setSollPosition(SERVO_BASE, BASE_HOVEROVERLAGER); //Base wird auf eine Tiefe Positon gefahren um Zeit zu Sparen.
            mAntrieb->moveAbsolutAngle(getPositionFromIndex(iLagerindex));
            if (inposition())
                iPickPackageStep++;
        }
        break;
        case 9: // Arm Runter
        {   
            //Base auf Ablageposition.
            setSollPosition(SERVO_BASE, BASE_LAGERPOSITION);
            if (inposition())
                iPickPackageStep++;
        }
        break;
        case 10: // Arm Greifer öffnen
        {   
            //Greifer öffnet
            setSollPosition(SERVO_GRIPP, GRIPP_OPEN_LAGER);
            if (inposition())
                iPickPackageStep++;
        }
        break;
        case 11: // Arm hoch home
        {   
            //Arm fährt hoch um sich drehen zu können
            setSollPosition(SERVO_BASE, BASE_HOVEROVERLAGER);
            if (inposition())
                iPickPackageStep++;
        }
        break;
        case 12: // Drehen Home
        {   
            //Drehtisch fährt auf die Parkposition
            mAntrieb->moveAbsolutAngle(DT_Parkposition);
            if (inposition())
                iPickPackageStep++;
        }
        break;
        case 13: // Rücksetzen der Schrittkette
        {   
            //man siehts
            iPickPackageStep = 0;
            setArmStatus(ArmStatus::AS_Ready);
            setPackStatus(PackStatus::STATUS_OK);
        }
    } 
}

/*
Paket wird vom gewünschten Lagerplatz geholt und wird vom Kunden oder am Ablageort platziert
Case 6 entscheidet über Kunde oder Ablageort
ab Case 100 nimmt der Kunde das Paket an
ab Case 200 wird das Paket beim Ablageort platziert
Ablageart definieren false=Kunde true=Ablageort
*/
void Greifer::PlacePackage()
{
    switch (iPlacePackageStep)
    {
        case 0: // Drehen
        {
            //Arm wird in die Position zum aufnehmen eines Paketes bewegt.
            uint32_t iTableAnglePlace = getPositionFromIndex(iLagerindex);
            mAntrieb->moveAbsolutAngle(iTableAnglePlace);
            //Weiterschaltbedingung
            if (inposition())
                iPlacePackageStep++;
        }
        break; 
        case 1: // Arm runter
        {   
            //Arm bewegt sich zur aufnahme Position
            setSollPosition(SERVO_GRIPP, GRIPP_GESCHLOSSEN);
            setSollPosition(SERVO_BASE, BASE_LAGERPOSITIONAUFNAHME);
            if (inposition())
                iPlacePackageStep++;
        }
        break;
        case 2: // Greifen
        {
            //Gripper setzt zupacken ein
            //sehr effektiv
            setSollPosition(SERVO_GRIPP, GRIPP_GESCHLOSSEN);
            if (inposition())
                iPlacePackageStep++;
        }
        break; 
        case 3: // Arm hoch
        {   
            // Arm fährt hoch um sich frei drehen zu können
            //ist der finger oben wird man dich loben
            setSollPosition(SERVO_BASE, BASE_HOVEROVERLAGER);
            if (inposition())
                iPlacePackageStep++;
        }
        break;
        case 4: // drehen Freigabe Position
        {
            //aber nicht zu schnell Jacqueline sonst musst du wieder kotzen
            //wird in die Sichere Position gedreht.
            mAntrieb->moveAbsolutAngle(DT_Bewegung_Freigegeben);
            setSollPosition(SERVO_BASE, BASE_OBEN);
            if (inposition())
                iPlacePackageStep++;
        }
        break; 
        case 5: // drehen
        {   
            //dreht sich auf die Abgabe Position
            mAntrieb->moveAbsolutAngle(DT_Greifposition);
            if (inposition())
                iPlacePackageStep++;
        }
        break;
        case 6: // Arm runter
        {
            //Arm bewegt sich zur abgabe Position
            //#könnte: man könnte auch die annahme Position vom Kunden bei der Position DT_Bewegung_Freigegeben machen.
            setSollPosition(SERVO_BASE, BASE_GEIFERPOSITION); 
            if (inposition())
                PaketKundeOderAblageort ? iPlacePackageStep = 200 : iPlacePackageStep = 100;
        }
        break;
        //================================================== Paket Kunde ==================================================
        case 100: // Annahme vom Kunden
        {   
            setPackStatus(PackStatus::STATUS_WaitingForCustomer);
            //if (PaketAnnahmeBestätigungKunde)
                iPlacePackageStep++;
        } 
        break;
        case 101: // Greifer öffnen Kunde
        {
            // Kunde hat bestätigt Paket wird übergeben
            //Sprung zurück auf Case 7 Homing ausführen
            setSollPosition(SERVO_GRIPP, GRIPP_OFFEN);
            if (inposition())
                iPlacePackageStep = 7;
        }
        break;
        //================================================== Paket Ablageort ==================================================
        case 200: // vorfahren
        {
            setPackStatus(PackStatus::STATUS_ReadyToMove);
            //Roboter fährt zum Paket um es mit dem Greifer ablegen zu können.
            //mNavigation->setSollPosition(mNavigation->getPosition()->getX(), mNavigation->getPosition()->getY() + 4);
            if (mNavigation->getDrivingState() == DrivingState::DRIVE_STATE_FINISHED)
                iPlacePackageStep++;
        }
        break;
        case 201: // Greifer öffnen
        {
            setPackStatus(PackStatus::STATUS_Undefined);
            //Greifer öffnet und Platziert das Paket
            setSollPosition(SERVO_GRIPP, GRIPP_OFFEN);
            if (inposition())
                iPlacePackageStep++;
        }
        break;
        case 202: // zurückfahren
        {
            setPackStatus(PackStatus::STATUS_ReadyToMove);
            //Roboter fährt zurück damit der Arm wieder sich frei bewegen kann.
            //Sprung zurück auf Case 7 Homing ausführen
            //mNavigation->setSollPosition(mNavigation->getPosition()->getX(), mNavigation->getPosition()->getY() - 4);
            if (mNavigation->getDrivingState() == DrivingState::DRIVE_STATE_FINISHED)
                iPlacePackageStep = 7;
        }
        break;
        //================================================== Homing ==================================================
        case 7: // Arm hoch 
        {
            setPackStatus(PackStatus::STATUS_Undefined);
            //Arm hoch zum freien drehen
            setSollPosition(SERVO_BASE, BASE_OBEN); 
            if (inposition())
                iPlacePackageStep++;
        }
        break;
        case 8: // Drehen Freigabeposition
        {
            // Drehen auf die freigabe Position
            mAntrieb->moveAbsolutAngle(DT_Bewegung_Freigegeben);
            if (inposition())
                iPlacePackageStep++;
        }
        break;
        case 9: // Drehen
        {
            //
            mAntrieb->moveAbsolutAngle(DT_Parkposition);
            setSollPosition(SERVO_BASE, BASE_HOVEROVERLAGER); 
            if (inposition())
                iPlacePackageStep++;
        }
        break;
        case 10: // Rücksetzen der Schrittkette
        {
            iPlacePackageStep = 0;
            setArmStatus(ArmStatus::AS_Ready);
            setPackStatus(PackStatus::STATUS_OK);
        }
        break;
    
    }
}


Drehtisch_Position Greifer::getPositionFromIndex(uint8_t index)
{
    //sLogger.debug("GetPositionFromIndex %u", index);

    switch (index)
    {
        case 1:
            return Drehtisch_Position::DT_Lager_PS1;
        case 2:
            return Drehtisch_Position::DT_Lager_PS2;
        case 3:
            return Drehtisch_Position::DT_Lager_PS3;
        case 4:
            return Drehtisch_Position::DT_Lager_PS4;
    }

    return Drehtisch_Position::DT_Parkposition;
}