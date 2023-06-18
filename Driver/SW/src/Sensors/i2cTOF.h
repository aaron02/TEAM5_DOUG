/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

enum Sensors
{
    POS_Back = 0,
    POS_Front,
    POS_Back_Left,
    POS_Front_Left
};

class Vector2D;

class i2cTOF : public Updateable
{
public:
    i2cTOF();
    ~i2cTOF();
    void Update(uint64_t difftime) override;

    void readSensorAtChannel(int channel);

    uint16_t getDistanceFromSensor(int index) { return distances[index]; }

    Vector2D* getInitialPosition(float &angle);

private:

protected:
    // Timer
    int32_t updateTimer = 10 * TimeVar::Millis;
    int32_t timer = 1 * TimeVar::Seconds;

    int16_t distances[4] = {0, 0, 0, 0};

    // Debug
    bool debug = false;
};