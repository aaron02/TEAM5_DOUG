/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "Navigation.h"

Navigation::Navigation(DriveTrain* drives, Odometry* odo) : m_Drive(drives), m_Odometry(odo)
{
    mSollPosition = new Vector2D(0, 0);
}

Navigation::~Navigation()
{

}

void Navigation::Update(uint64_t difftime)
{
    // Check Position Each Second should be enough
    if (timer <= 0)
    {
        timer = 50 * TimeVar::Millis;

        if (m_Odometry->GetPosition() != getSollPosition())
        {
            Vector2D* mPosition = m_Odometry->GetPosition();

            float xDifference = mPosition->getX() - mSollPosition->getX();
            float yDifference = mPosition->getY() - mSollPosition->getY();

            // Distanz Luftline
            float distance = sqrtf(yDifference * yDifference + xDifference * xDifference);

            if (debug)
                sLogger.debug("xDif %f, yDif %f, Distance %f", xDifference, yDifference, distance);

            // Gleichzeitiges Fahren
            if (0)
            {

            fSpeedX = calculateSpeed(xDifference);
            fSpeedY = calculateSpeed(yDifference);

            //sLogger.debug("xSpeed = %f, ySpeed = %f", fSpeedX, fSpeedY);
            m_Drive->Drive(fSpeedX, fSpeedY, 0.0f, m_Odometry->getHeading());
            }
            else
            {
                if (yDifference > 0.5 || yDifference < -0.5)
                {
                    // Fahre zuerst Y Richtung
                    fSpeedX = 0.0f;
                    fSpeedY = calculateSpeed(yDifference);

                    if (debug)
                        sLogger.debug("xSpeed = %f, ySpeed = %f", fSpeedX, fSpeedY);

                    m_Drive->Drive(fSpeedX, fSpeedY, 0.0f, m_Odometry->getHeading());
                }
                else
                {
                    if (xDifference > 0.5 || xDifference < -0.5)
                    {
                    // Fahre X Richtung
                    fSpeedX = calculateSpeed(xDifference);
                    fSpeedY = 0.0f;

                    if (debug)
                        sLogger.debug("xSpeed = %f, ySpeed = %f", fSpeedX, fSpeedY);

                    m_Drive->Drive(fSpeedX, fSpeedY, 0.0f, m_Odometry->getHeading());
                    }
                    else
                    {
                        // Position Erreicht
                        if (debug)
                            sLogger.debug("Position Erreicht");

                        m_Drive->Drive(0.0f, 0.0f, 0.0f);
                    }
                }
            }
        }
        else
        {
            //sLogger.debug("Position Erreicht");
            m_Drive->Drive(0.0f, 0.0f, 0.0f);
        }
    }
    else
        timer -= difftime;
}

void Navigation::setSollPosition(float x, float y)
{
     mSollPosition->changeCoords(x, y);
}

float Navigation::calculateSpeed(float distance) 
{
    // Maximale Distanz (entspricht der Entfernung, bei der die Geschwindigkeit 0 erreicht)
    const float maxDistance = 200.0;

    // Berechnung der Geschwindigkeit
    float speed = (distance / maxDistance) * maxSpeed * -1;

    // Niemels unter 0.25
    if (speed != 0 && (speed < 0.25 && speed > 0))
        speed = 0.25;

    if (speed != 0 && (speed > -0.25 && speed < 0))
        speed = -0.25;

    // Begrenze die Geschwindigkeit innerhalb des zulässigen Bereichs
    if (speed > maxSpeed) 
    {
        speed = maxSpeed;
    } 
    else if (speed < -maxSpeed)
    {
        speed = -maxSpeed;
    }

    return speed;
}

void Navigation::abortDriving()
{
    // Soll zu Istposition sollte genügen um die Motoren zu stoppen
    mSollPosition->changeCoords(m_Odometry->GetPosition()->getX(), m_Odometry->GetPosition()->getY());
}
