/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "Antrieb.h"

Antrieb::Antrieb(std::string sName, uint32_t iStep,uint32_t iDir,uint32_t iEn)
{
    sLogger.info("Controller Initialize Antrieb %s", sName.c_str());

    // Axis Initialisation
    stepper = new AccelStepper(AccelStepper::DRIVER, iStep, iDir);
    pinMode(iEn, OUTPUT);

    sAntriebName = sName;
    iStepPin = iStep;
    iDirPin = iDir;
    iEnablePin = iEn;
    
    // Default Speeds
    stepper->setMaxSpeed(500.0f);
    stepper->setSpeed(200.0f);
    stepper->setAcceleration(10.0f);

    // DEBUG INFO
    sLogger.debug("Pins used For %s: %s", sAntriebName.c_str(), getPinsNameString().c_str());
}

Antrieb::~Antrieb()
{
    delete stepper;
}

void Antrieb::Update(uint64_t difftime)
{
    if (getSpeed() < 0.0 || getSpeed() > 0.0)
    {
        stepper->runSpeed();
    }
}

double Antrieb::mapDouble(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

void Antrieb::setSpeed(float fSpeed)
{
    fDemandedSpeed = mapDouble(fSpeed, -1.0, 1.0, -2000.0, 2000.0);

    // Only for Debug use, activating this Line Stops the Motors from Working, this extends the Program Cycle.
    //sLogger.info("Antrieb %s set to Speed %f (%f)", sAntriebName.c_str(), fSpeed, fDemandedSpeed);

    if (stepper->speed() < fDemandedSpeed || stepper->speed() > fDemandedSpeed)
        stepper->setSpeed(fDemandedSpeed);
}

float Antrieb::getSpeed()
{
    return fDemandedSpeed;
}

std::string Antrieb::getPinsNameString()
{
    std::stringstream ss;

    ss << "StepPin = ";
    ss << iStepPin;
    ss << " ";
    ss << "DirPin = ";
    ss << iDirPin;
    ss << " ";
    ss << "EnablePin = ";
    ss << iEnablePin;

    return ss.str();
}