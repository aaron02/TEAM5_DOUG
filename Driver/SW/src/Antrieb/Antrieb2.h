/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

class Updateable;

class Antrieb2 : public Updateable
{
public:
    Antrieb2(std::string sName, uint32_t iStep, uint32_t iFLDir, uint32_t iFRDir, uint32_t iBLDir, uint32_t iBRDir, uint32_t iEn);
    ~Antrieb2();

    // Cyclyc Update
    void Update(uint64_t difftime) override;

    // Speed Functions
    void setSpeed(MotorType iMotor, float fSpeed);
    float getSpeed();

    void step();

    void disableOutputs();
    void enableOutputs();

    std::string getPinsNameString();

private:
    double mapDouble(double x, double in_min, double in_max, double out_min, double out_max);

    // Speed to Run the Motors
    std::vector<float> fDemandedSpeed = {0, 0, 0, 0};

    // Step Interval Pulses per microseconds
    unsigned long _stepInterval = 0;
    // The last step time in microseconds
    unsigned long  _lastStepTime;

    std::string sAntriebName = "";
    uint32_t iStepPin = 0;
    uint32_t iDirPinFL = 0;
    uint32_t iDirPinFR = 0;
    uint32_t iDirPinBL = 0;
    uint32_t iDirPinBR = 0;
    uint32_t iEnablePin = 0;
};