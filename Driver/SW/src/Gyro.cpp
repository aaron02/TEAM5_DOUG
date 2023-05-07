/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "Gyro.h"

Gyro::Gyro()
{
    sLogger.debug("Gyro Disabled due to Shitty Part");
}

Gyro::~Gyro()
{

}

void Gyro::Update(uint64_t difftime)
{
    // Update Sensor evry 10 ms
    if (sensorUpdate < 0)
    {
        //imu.update();

        sensorUpdate = 10 * TimeVar::Millis;;
    }
    else
        sensorUpdate = sensorUpdate - difftime;

    // Output Info evry 2 s
    if (timer < 0)
    {
        //sLogger.info("Pitch: %f Roll: %f Yaw: %f", getGyroAngle(PITCH), getGyroAngle(ROLL), getGyroAngle(YAW));
        timer = 2 * TimeVar::Seconds;
    }
    else
        timer = timer - difftime;
}

float Gyro::getGyroAngle(GYRO_AXIS iAxis)
{
    float angle = 0.0;
    /*
    switch (iAxis)
    {
        case PITCH:
            angle = imu.getAngleX();
        break;
        case ROLL:
            angle = imu.getAngleY();
        break;
        case YAW:
            angle = imu.getAngleZ();
        break;
    }*/

    return angle;
}
