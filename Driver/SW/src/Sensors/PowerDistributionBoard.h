/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

class PDB
{
public:
    PDB();
    ~PDB();

    // Cyclyc Update
    void Update(uint64_t difftime);

    // ready our Sensordata
    void ReadSensor();

    // returns the current drawn at the moment
    float GetCurrent() { return fCurrent; }

    // returns the current drawn since we startet our Robot
    float GetCurrentDrawn() { return fCurrentDrawn; }

    // returns the current voltage
    float GetVoltage() { return fVoltage; }

    // returns the current voltage in pct in regards to the starting voltage
    float GetVoltagePct() { return ((fVoltage / fBatteryStartVoltage) * 100); }

    // returns our voltage drawn sinse we startet our Robot
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

    bool debug = false;
};
