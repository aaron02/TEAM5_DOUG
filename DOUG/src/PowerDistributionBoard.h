/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Defnies.h"

class PDB
{
public:
    PDB();
    ~PDB();

    void Update(uint64_t difftime);
    void ReadSensor();

    float GetCurrent() { return fCurrent; }
    float GetCurrentDrawn() { return fCurrentDrawn; }
    float GetVoltage() { return fVoltage; }
    float GetVoltageDrawn() { return fVoltageDrawn; }

private:
    int32_t sensorUpdate = 100 * TimeVar::Millis;
    int32_t timer = 1 * TimeVar::Seconds;

protected:
    float fCurrent = 0.0f;
    float fVoltage = 0.0f;

    float fBatteryStartVoltage = 0.0f;

    // Current Used since Power On
    float fCurrentDrawn = 0.0f;

    // Voltage Used since Power On
    float fVoltageDrawn = 0.0f;

    // Analog Constants
    float fMinVoltage = 0.0f;
    float fMaxVoltage = 60.0f;

    float fMinCurrent = 0.0f;
    float fMaxCurrent = 440.0f;
};
