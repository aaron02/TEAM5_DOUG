/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "Greifer.h"

Greifer::Greifer(uint8_t servoPin1, uint8_t servoPin2, PosAntrieb& dreh, Navigation& nav) : mAntrieb(&dreh), mNavigation(&nav)
{
    servo[SERVO_BASE].attach(servoPin1);
    servo[SERVO_GRIPP].attach(servoPin2);
    GS_INIT_FIRST_TIME ++;

}

Greifer::~Greifer()
{
    servo[SERVO_BASE].detach();
    servo[SERVO_GRIPP].detach();
}

void Greifer::Update(uint64_t difftime)
{
    if(GS_INIT_FIRST_TIME==1)
    {
        Grundstellung();
    }
    // Servo 1
    runServo(SERVO_BASE, difftime);

    // Servo 2
    runServo(SERVO_GRIPP, difftime);

}

//Prüft ob alle Aktoren die gewünschte Position erreicht
//t=ture f=false
inposition Greifer::inposition(){
    if((getPosition(SERVO_BASE) == getSollPosition(SERVO_BASE)) && (getPosition(SERVO_GRIPP) == getSollPosition(SERVO_GRIPP)) && (mAntrieb->inPosition())){
        return t;
    };
    if(!((getPosition(SERVO_BASE) == getSollPosition(SERVO_BASE)) && (getPosition(SERVO_GRIPP) == getSollPosition(SERVO_GRIPP)) && (mAntrieb->inPosition()))){
        return f;
    };
};

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
Grundstellung Greifer::Grundstellung()
{
    int step = 1;

    switch (step)
    {
        case 1:
        {
            /*
            Base und Gripp werden in einen Sicheren Bereich gefahren. Weiterschaltung nur über Base Zeilposition
            weil nur er eine Kollision erfahren kann. IF-Bedingung soll den nächsten Case Starten wenn Position erreicht ist.
            */
            setSollPosition(SERVO_BASE, BASE_GS);
            setSollPosition(SERVO_GRIPP, GRIPP_OFFEN);
            runServo(SERVO_BASE, 1000);
            runServo(SERVO_GRIPP, 1000);

            //Weiterschaltbedingung
            if ((getPosition(SERVO_BASE) == getSollPosition(SERVO_BASE))){
                step++; };

            return GS_Running;
        }
        break;
        case 2:
        {
            // Drehtisch wird referenziert. Nächster Schritt wenn Done.

            if(mAntrieb->isHomed()==false){
                mAntrieb->setHoming();
            };

            //Weiterschaltbedingung
            if (mAntrieb->isHomed()==true){   // #try hoffe das geht so
                step++; };

            return GS_Running;
        }
        break;
        case 3:
        {
            // Drehtisch und SERVO_BASE gehen in die Parkposition.

            
            mAntrieb->moveAbsolutAngle(DT_Parkposition);
            setSollPosition(SERVO_BASE, BASE_HOVEROVERLAGER);
            runServo(SERVO_BASE, 1000);

            //Weiterschaltbedingung
            if ((mAntrieb->inPosition()) && (getPosition(SERVO_BASE) == getSollPosition(SERVO_BASE))){
                step++; };

            return GS_Running;
        }
        break;
        case 4:
        {
            // Grundstellung abgeschlossen. Rücksetzten von der INIT variable
            GS_INIT_FIRST_TIME = 0;
            
            return GS_OK;
        }
        break;
    }

}

void Greifer::setArmStatus()
{

}

ArmStatus Greifer::getArmStatus()
{
    return ArmStatus::READY;
}

PackStatus Greifer::PickPackage(Drehtisch_Position lagerIndex)
{
        int step = 1;

        switch (step)
    {
        case 1: // Arm Rauf
        {
            //Funktion
            setSollPosition(SERVO_GRIPP, GRIPP_OFFEN);
            runServo(SERVO_GRIPP,1000);
            setSollPosition(SERVO_BASE, BASE_OBEN);
            runServo(SERVO_BASE,1000);
            if (inposition()==t)
            {
                step++;
            }
            
        }
        break; // Drehen
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

PackStatus Greifer::PlacePackage(uint8_t lagerIndex)
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

