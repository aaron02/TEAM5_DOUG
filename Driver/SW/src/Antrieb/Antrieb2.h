/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

class Antrieb2
{
public:
    Antrieb2(std::string sName, uint32_t iStep, uint32_t iFLDir, uint32_t iFRDir, uint32_t iBLDir, uint32_t iBRDir, uint32_t iEn);
    ~Antrieb2();

    void Update(uint64_t difftime);

    // Speed Functions
    void setSpeed(MotorType iMotor, float fSpeed);
    float getSpeed();

    void disableOutputs();
    void enableOutputs();

    std::string getPinsNameString();

private:
    double mapDouble(double x, double in_min, double in_max, double out_min, double out_max);

    // Speed to Run the Motors
    std::vector<float> fDemandedSpeed = {0, 0, 0, 0};

    std::string sAntriebName = "";
    uint32_t iStepPin = 0;
    uint32_t iDirPinFL = 0;
    uint32_t iDirPinFR = 0;
    uint32_t iDirPinBL = 0;
    uint32_t iDirPinBR = 0;
    uint32_t iEnablePin = 0;
};