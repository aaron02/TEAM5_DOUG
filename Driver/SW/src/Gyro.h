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
    struct euler
    {
    float yaw;
    float pitch;
    float roll;
    } ypr;

public:
    Gyro();
    ~Gyro();

    void setReports(sh2_SensorId_t reportType, long report_interval);
    void quaternionToEulerRV(sh2_RotationVectorWAcc_t* rotational_vector, euler* ypr, bool degrees) ;
    void quaternionToEuler(float qr, float qi, float qj, float qk, euler* ypr, bool degrees);

    void Update(uint64_t difftime);

    float getGyroAngle(GYRO_AXIS iAxis);

private:
    int32_t sensorUpdate = 10 * TimeVar::Millis;
    int32_t timer = 1 * TimeVar::Seconds;

protected:
    long reportIntervalUs = 5000;
    sh2_SensorId_t reportType = SH2_ARVR_STABILIZED_RV;

    bool debug = false;
};