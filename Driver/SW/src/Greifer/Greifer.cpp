/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "Greifer.h"

Greifer::Greifer(uint8_t servoPin1, uint8_t servoPin2, PosAntrieb& dreh, Navigation& nav) : mAntrieb(&dreh), mNavigation(&nav)
{
    servo[SERVO_BASE].attach(servoPin1);
    servo[SERVO_GRIPP].attach(servoPin2);
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
    return (getPosition(SERVO_BASE) == getSollPosition(SERVO_BASE)) && (getPosition(SERVO_GRIPP) == getSollPosition(SERVO_GRIPP)) && (mAntrieb->inPosition());
}

void Greifer::runServo(ServoMapping servoIndex, uint64_t difftime)
{
    if (getPosition(servoIndex) != getSollPosition(servoIndex))
    {
        if (updateTimer[servoIndex] <= 0)
        {
            setTimer(servoIndex, 500); //1000:500 -> 2 grad/s

            int8_t value = (getPosition(servoIndex) < getSollPosition(servoIndex)) ? 1 : -1;

            // Servo Um 1 Grad Bewegen
            servo[servoIndex].write(getPosition(servoIndex) + value);
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
            mPosition = servo[SERVO_BASE].read();
        } break;
        case SERVO_GRIPP:
        {
            mPosition = servo[SERVO_GRIPP].read();
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

void Greifer::setSollPosition(ServoMapping servo, SERVO_Positionen degree) //uint8_t degree 
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
    return;
    
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
            setSollPosition(SERVO_BASE, BASE_HOVEROVERLAGER);

            //Weiterschaltbedingung
            if ((mAntrieb->inPosition()) && (getPosition(SERVO_BASE) == getSollPosition(SERVO_BASE)))
                iGrundstellungStep++;
        } break;
        case 3:
        {
            // Grundstellung abgeschlossen. Rücksetzten von der INIT variable
            setArmStatus(ArmStatus::AS_Ready);
        } break;
    }
}

void Greifer::setArmStatus(ArmStatus state)
{
    iGripperState = state;
}

ArmStatus Greifer::getArmStatus()
{
    return iGripperState;
}

PackStatus Greifer::PickPackage()
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
        break; // Drehen
        case 1:
        {
            //Funktion
            uint32_t iTableAngle = getPositionFromIndex(iLagerindex);
            mAntrieb->moveAbsolutAngle(iTableAngle);
        }
        break;
        case 2:
        {
            //Funktion
            mNavigation->setSollPosition(3, 0);
            if (mNavigation->getDrivingState() == DrivingState::DRIVE_STATE_FINISHED)
                iPickPackageStep++;
        }
        break;
        case 3:
        {
            //Funktion
        }
        break;
    } 
    // Arm Rauf
    // Drehen
    // Arm Runter
    // Greiffen
    // Arm Hoch
    // Drehen
    // Arm Runter
    // Greiffer öffnen
    // Home
    return PackStatus::STATUS_OK;
}

PackStatus Greifer::PlacePackage()
{
    return PackStatus::STATUS_OK;
}

/*
    int step = 1;

        switch (step)
    {
        case 1:
        {
            //Funktion
        }
        break;
        case 2:
        {
            //Funktion
        }
        break;
        case 3:
        {
            //Funktion
        }
        break;
        case 4:
        {
            //Funktion
        }
        break;
    }

*/

Drehtisch_Position Greifer::getPositionFromIndex(uint8_t index)
{
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