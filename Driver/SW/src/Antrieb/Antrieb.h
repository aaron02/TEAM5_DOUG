/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

class Updateable;

class Antrieb : public Updateable
{
    public:
    Antrieb(std::string sName, uint32_t iStep, uint32_t iDir, uint32_t iEn);
    ~Antrieb();

    // Cyclyc Update
    void Update(uint64_t difftime) override;

    // Speed Functions
    void setSpeed(float fSpeed);
    float getSpeed();

    std::string getPinsNameString();

    private:
    // Stepper Axis
    AccelStepper* stepper;

    double mapDouble(double x, double in_min, double in_max, double out_min, double out_max);

    // Speed to Run the Motor
    float fDemandedSpeed = 0;

    std::string sAntriebName = "";
    uint32_t iStepPin = 0;
    uint32_t iDirPin = 0;
    uint32_t iEnablePin = 0;
};