/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

class i2cTOF : public Updateable
{
public:
    i2cTOF();
    ~i2cTOF();
    void Update(uint64_t difftime) override;

    void readSensorAtChannel(int channel);

    uint16_t getDistanceFromSensor(int index) { return distances[index]; }

private:

protected:
    // Timer
    int32_t updateTimer = 10 * TimeVar::Millis;
    int32_t timer = 1 * TimeVar::Seconds;

    int16_t distances[4] = {0, 0, 0, 0};

    // Debug
    bool debug = false;
};