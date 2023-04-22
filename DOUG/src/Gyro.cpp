/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "Gyro.h"

MPU6050 mpu(Wire);

Gyro::Gyro()
{
    byte status = mpu.begin();

    // Initialize MPU6050
    if (status != 0) 
        sLogger.failure("Failed to find MPU6050 chip");

    sLogger.debug("MPU6050 Found and Initialized");

    // MPU Setup Motion Detection
    mpu.calcOffsets();
}

Gyro::~Gyro()
{

}

void Gyro::Update(uint64_t difftime)
{
    // Update Sensor evry 10 ms
    if (sensorUpdate < 0)
    {
        mpu.update();
        sensorUpdate = 10 * TimeVar::Millis;;
    }
    else
        sensorUpdate = sensorUpdate - difftime;

    // Output Info evry 2 s
    if (timer < 0)
    {
        //sLogger.info("Pitch: %f Roll: %f Yaw: %f", mpu.getAngleX(), mpu.getAngleY(), mpu.getAngleZ());
        sLogger.info("Pitch: %f Roll: %f Yaw: %f", getGyroAngle(PITCH), getGyroAngle(ROLL), getGyroAngle(YAW));
        timer = 2 * TimeVar::Seconds;
    }
    else
        timer = timer - difftime;
}

float Gyro::getGyroAngle(GYRO_AXIS iAxis)
{
    float angle = 0.0;

    switch (iAxis)
    {
        case PITCH:
            angle = mpu.getAngleX();
        break;
        case ROLL:
            angle = mpu.getAngleY();
        break;
        case YAW:
            angle = mpu.getAngleZ();
        break;
    }

    return angle;
}
