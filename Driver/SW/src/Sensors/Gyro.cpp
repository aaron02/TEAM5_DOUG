/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "Gyro.h"

// I2C mode
#define BNO08X_RESET -1

Adafruit_BNO08x  bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

Gyro::Gyro()
{
    // Try to initialize!
    if (!bno08x.begin_UART(&Serial4))  // Requires a device with > 300 byte UART buffer!
    {
        sLogger.debug("Gyro:: Failed to find BNO08x chip");
        while (1) { delay(10); }
    }

    sLogger.debug("Gyro:: BNO08x Found!");

    setReports(reportType, reportIntervalUs);

    sLogger.debug("Gyro:: Reading events");
    delay(100);
}

Gyro::~Gyro()
{

}

void Gyro::setReports(sh2_SensorId_t reportType, long report_interval) 
{
  sLogger.debug("Gyro:: Setting desired reports");

  if (!bno08x.enableReport(reportType, report_interval)) 
  {
    sLogger.debug("Gyro:: Could not enable stabilized remote vector");
  }
}

void Gyro::quaternionToEuler(float qr, float qi, float qj, float qk, euler* ypr, bool degrees = false)
{
    float sqr = sq(qr);
    float sqi = sq(qi);
    float sqj = sq(qj);
    float sqk = sq(qk);

    ypr->yaw = atan2(2.0 * (qi * qj + qk * qr), (sqi - sqj - sqk + sqr));
    ypr->pitch = asin(-2.0 * (qi * qk - qj * qr) / (sqi + sqj + sqk + sqr));
    ypr->roll = atan2(2.0 * (qj * qk + qi * qr), (-sqi - sqj + sqk + sqr));

    if (degrees) 
    {
      ypr->yaw *= RAD_TO_DEG;
      ypr->pitch *= RAD_TO_DEG;
      ypr->roll *= RAD_TO_DEG;
    }
}

void Gyro::quaternionToEulerRV(sh2_RotationVectorWAcc_t* rotational_vector, euler* ypr, bool degrees = false) 
{
    quaternionToEuler(rotational_vector->real, rotational_vector->i, rotational_vector->j, rotational_vector->k, ypr, degrees);
}

void Gyro::Update(uint64_t difftime)
{
    // Update Sensor evry 10 ms
    //if (sensorUpdate < 0)
    //{
        if (bno08x.wasReset()) 
        {
            sLogger.debug("Gyro:: sensor was reset ");
            setReports(reportType, reportIntervalUs);
        }

        if (bno08x.getSensorEvent(&sensorValue)) 
        {
            if (sensorValue.sensorId == SH2_ARVR_STABILIZED_RV)
            {
                quaternionToEulerRV(&sensorValue.un.arvrStabilizedRV, &ypr, true);
            }
        }

        sensorUpdate = 5 * TimeVar::Millis;;
    //}
    //else
    //    sensorUpdate = sensorUpdate - difftime;

    // Output Info evry 2 s
    if (debug)
    {
        if (timer < 0)
        {
            sLogger.info("Pitch: %f Roll: %f Yaw: %f", getGyroAngle(PITCH), getGyroAngle(ROLL), getGyroAngle(YAW));
            timer = 2 * TimeVar::Seconds;
        }
        else
            timer = timer - difftime;
    }
}

float Gyro::getGyroAngle(GYRO_AXIS iAxis)
{
    float angle = 0.0;
    
    switch (iAxis)
    {
        case PITCH:
            angle = ypr.pitch;
        break;
        case ROLL:
            angle = ypr.roll;
        break;
        case YAW:
            angle = ypr.yaw;
        break;
    }

    return angle;
}
