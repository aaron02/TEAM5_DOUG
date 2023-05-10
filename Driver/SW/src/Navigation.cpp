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
    if (moveTimer <= 0)
    {
        if (moveStep < 4)
        {
            moveTimer = 15 * TimeVar::Seconds;
            moveStep = moveStep + 1;

            switch (moveStep)
            {
                case 1:
                {
                    fSpeedX = 1.0f;
                    fSpeedY = 0.0f;
                }
                break;
                case 2:
                {
                    fSpeedX = 0.0f;
                    fSpeedY = 1.0f;
                }
                break;
                case 3:
                {
                    fSpeedX = -1.0f;
                    fSpeedY = 0.0f;
                }
                break;
                case 4:
                {
                    fSpeedX = 0.0f;
                    fSpeedY = -1.0f;
                }
                break;
            }

        }
        else
        {
            m_Drive->Drive(0.0, 0.0, 0.0, 0.0f);
            return;
        }
    }
    else
    {
        moveTimer = moveTimer - difftime;
    }

    m_Drive->Drive(fSpeedX, fSpeedY, 0.0f, m_Odometry->getGyro()->getGyroAngle(GYRO_AXIS::YAW));
}
