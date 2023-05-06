/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "PowerDistributionBoard.h"  
    
PDB::PDB()
{
    pinMode(A9, INPUT);
    pinMode(A8, INPUT);

    float fAnalogVoltage = analogRead(A9);
    fBatteryStartVoltage = map(fAnalogVoltage, 0, 885, fMinVoltage, fMaxVoltage);
}

PDB::~PDB()
{

}

void PDB::Update(uint64_t difftime)
{
    // Update Sensor evry 10 ms
    if (sensorUpdate <= 0)
    {
        ReadSensor();
        sensorUpdate = 10 * TimeVar::Millis;;
    }
    else
        sensorUpdate = sensorUpdate - difftime;

    // Output Info evry 1 s
    if (timer < 0)
    {
        sLogger.info("Current Voltage = %f used %f Amps",GetVoltage(), GetCurrentDrawn());

        timer = 2 * TimeVar::Seconds;
    }
    else
        timer = timer - difftime;
}

void PDB::ReadSensor()
{
    float fAnalogVoltage = analogRead(A9);
    float fAnalogCurrent = analogRead(A8);

    // Scaling Analog Values
    fVoltage = map(fAnalogVoltage, 0, 885, fMinVoltage, fMaxVoltage);
    fCurrent = map(fAnalogCurrent, 0, 1023, fMinCurrent, fMaxCurrent);

    // Used Voltage and Current
    fCurrentDrawn += (fCurrent / 1000.0f);
    fVoltageDrawn -= fBatteryStartVoltage - fVoltage;

    //sLogger.info("Current Voltage = %f Currentdraw = %f used %f Amps",GetVoltage(), GetCurrent(), GetCurrentDrawn());
}