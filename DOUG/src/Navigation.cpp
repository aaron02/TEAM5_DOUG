/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "Navigation.h"

Navigation::Navigation(DriveTrain* drives, Odometry* odo) : m_Drive(drives), m_Odometry(odo)
{
    
}

Navigation::~Navigation()
{

}

void Navigation::Update(uint64_t difftime)
{
    // Test Movement Quadratic Move.
    if (moveTimer < 0)
    {
        if (moveStep < 4)
        {
            moveTimer = 5 * TimeVar::Seconds;
            moveStep = moveStep + 1;
        }
        else
            m_Drive->Drive(0.0, 0.0, 0.0, 0.0f);
    }
    else
    {
        moveTimer = moveTimer - difftime;

        switch (moveStep)
        {
            case 1:
            {
                 m_Drive->Drive(1.0, 0.0, 0.0, m_Odometry->getGyro()->getGyroAngle(GYRO_AXIS::YAW));
            }
            break;
            case 2:
            {
                 m_Drive->Drive(0.0, 1.0, 0.0, m_Odometry->getGyro()->getGyroAngle(GYRO_AXIS::YAW));
            }
            break;
            case 3:
            {
                 m_Drive->Drive(-1.0, 0.0, 0.0, m_Odometry->getGyro()->getGyroAngle(GYRO_AXIS::YAW));
            }
            break;
            case 4:
            {
                 m_Drive->Drive(0.0, -1.0, 0.0, m_Odometry->getGyro()->getGyroAngle(GYRO_AXIS::YAW));
            }
            break;
        }
    }
}
