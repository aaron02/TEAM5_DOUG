/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Defnies.h"

enum GYRO_AXIS : uint8_t
{
    PITCH,
    ROLL,
    YAW
};

class Gyro
{
    public:
    Gyro();
    ~Gyro();

    void Update(uint64_t difftime);

    float getGyroAngle(GYRO_AXIS iAxis);

    private:
    int32_t sensorUpdate = 10 * TimeVar::Millis;
    int32_t timer = 1 * TimeVar::Seconds;
};