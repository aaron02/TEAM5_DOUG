/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "Antrieb2.h"

Antrieb2::Antrieb2(std::string sName, uint32_t iStep, uint32_t iFLDir, uint32_t iFRDir, uint32_t iBLDir, uint32_t iBRDir, uint32_t iEn)
{
    sLogger.info("Controller Initialize Antrieb %s", sName.c_str());

    // Axis Initialisation
    pinMode(iStep, OUTPUT);
    pinMode(iFLDir, OUTPUT);
    pinMode(iFRDir, OUTPUT);
    pinMode(iBLDir, OUTPUT);
    pinMode(iBRDir, OUTPUT);
    pinMode(iEn, OUTPUT);

    sAntriebName = sName;
    iStepPin = iStep;
    iDirPinFL = iFLDir;
    iDirPinFR = iFRDir;
    iDirPinBL = iBLDir;
    iDirPinBR = iBRDir;
    iEnablePin = iEn;

    // DEBUG INFO
    sLogger.debug("Pins used For %s: %s", sAntriebName.c_str(), getPinsNameString().c_str());
}

Antrieb2::~Antrieb2()
{

}

void Antrieb2::Update(uint64_t difftime)
{
    if (getSpeed() < 0.0 || getSpeed() > 0.0)
    {
        // Pulse Here
    }
}

double Antrieb2::mapDouble(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

void Antrieb2::setSpeed(MotorType iMotor, float fSpeed)
{
    fDemandedSpeed[iMotor] = mapDouble(fSpeed, -1.0, 1.0, -50.0, 50.0);

    switch (iMotor)
    {
        case MotorType::kFrontLeft:
        {
            if (fDemandedSpeed[iMotor] < 0.0)
                digitalWrite(iDirPinFL, HIGH);
            else
                digitalWrite(iDirPinFL, LOW);
                
        } break;
        case MotorType::kFrontRight:
        {
            if (fDemandedSpeed[iMotor] < 0.0)
                digitalWrite(iDirPinFR, HIGH);
            else
                digitalWrite(iDirPinFR, LOW);

        } break;
        case MotorType::kRearLeft:
        {
            if (fDemandedSpeed[iMotor] < 0.0)
                digitalWrite(iDirPinBL, HIGH);
            else
                digitalWrite(iDirPinBL, LOW);

        } break;
        case MotorType::kRearRight:
        {
            if (fDemandedSpeed[iMotor] < 0.0)
                digitalWrite(iDirPinBR, HIGH);
            else
                digitalWrite(iDirPinBR, LOW);

        } break;
    }

    // Only for Debug use, activating this Line Stops the Motors from Working, this extends the Program Cycle.
    //sLogger.info("Antrieb %s set to Speed %f (%f)", sAntriebName.c_str(), fSpeed, fDemandedSpeed);
}

float Antrieb2::getSpeed()
{
    // Each Axis Has the Same Speed ;)
    return fDemandedSpeed[0];
}

void Antrieb2::disableOutputs()
{   
    digitalWrite(iEnablePin, LOW);
}

void Antrieb2::enableOutputs()
{
    digitalWrite(iEnablePin, HIGH);
}

std::string Antrieb2::getPinsNameString()
{
    std::stringstream ss;

    ss << "StepPin = ";
    ss << iStepPin;
    ss << " ";
    ss << "DirPin FL = ";
    ss << iDirPinFL;
    ss << " ";
    ss << "DirPin FR = ";
    ss << iDirPinFR;
    ss << " ";
    ss << "DirPin BL = ";
    ss << iDirPinBL;
    ss << " ";
    ss << "DirPin BR = ";
    ss << iDirPinBR;
    ss << " ";
    ss << "EnablePin = ";
    ss << iEnablePin;

    return ss.str();
}