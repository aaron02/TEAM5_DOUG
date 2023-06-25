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

        if (getDrivingState() == DRIVE_STATE_BUSY)
        {
            Vector2D* mPosition = m_Odometry->GetPosition();

            float xDifference = mPosition->getX() - mSollPosition->getX();
            float yDifference = mPosition->getY() - mSollPosition->getY();

            // Distanz Luftline
            float distance = sqrtf(yDifference * yDifference + xDifference * xDifference);

            // Winkel zum Ziel
            float targetAngle = calculateAngle(mSollPosition->getX(), mSollPosition->getY(), mPosition->getX(), mPosition->getY());
            float remainingAngle = calculateRemainingAngle(m_Odometry->getHeading(), targetAngle);

            if (debug)
                sLogger.debug("xDif %f, yDif %f, Distance %f", xDifference, yDifference, distance);

            // We are Further than 200mm away face our Target.
            if ((distance > 200.0) && (remainingAngle > 0.5 || remainingAngle < -0.5))
            {
                
                float turnSpeed = getTurnSpeed(remainingAngle / 180.0);

                if (debug)
                    sLogger.debug("rotSpeed = %f, ActualAngle = %f, RemainingAngle = %f", turnSpeed, m_Odometry->getHeading(), remainingAngle);

                // Roboter Drehen
                m_Drive->Drive(0.0f, 0.0f, turnSpeed, m_Odometry->getHeading());
                return;
            }
            
            /////////////////////////////////////////////////////////////////////////////
            //  In Position Checking
            /////////////////////////////////////////////////////////////////////////////
            if (xDifference <= 5.5 && xDifference >= -5.5)
            {
                if (!xFirst)
                    xFinished = true;
            }
            else
                xFinished = false;

            if (yDifference <= 5.5 && yDifference >= -5.5)
            {
                if (!yFirst)
                    yFinished = true;
            }
            else
                yFinished = false;

            if (xFinished && yFinished)
            {
                xFirst = false;
                yFirst = false;

                // Position Erreicht
                if (debug)
                    sLogger.debug("Position Erreicht");

                setDrivingState(DRIVE_STATE_FINISHED);
                m_Drive->Drive(0.0f, 0.0f, 0.0f);
            }
            /////////////////////////////////////////////////////////////////////////////

            // Drive First Whats Further
            if ((xDifference > 200 || xFinished != true) && yFirst != true)
            {
                xFirst = true;

                if (xDifference > 2.5 || xDifference < -2.5)
                {
                    setDrivingState(DRIVE_STATE_BUSY);

                    // Fahre X Richtung
                    fSpeedX = calculateSpeed(xDifference);
                    fSpeedY = 0.0f;

                    if (debug)
                        sLogger.debug("xSpeed = %f, ySpeed = %f", fSpeedX, fSpeedY);

                    m_Drive->Drive(fSpeedX, fSpeedY, 0.0f, m_Odometry->getHeading());
                }
                else
                    xFirst = false;
            }
            else if ((yDifference > 200 || yFinished != true) && xFirst != true)
            {
                yFirst = true;

                if (yDifference > 2.5 || yDifference < -2.5 )
                {
                    setDrivingState(DRIVE_STATE_BUSY);

                    // Fahre zuerst Y Richtung
                    fSpeedX = 0.0f;
                    fSpeedY = calculateSpeed(yDifference) * -1;

                    if (debug)
                        sLogger.debug("xSpeed = %f, ySpeed = %f", fSpeedX, fSpeedY);

                    m_Drive->Drive(fSpeedX, fSpeedY, 0.0f, m_Odometry->getHeading());
                }
                else
                    yFirst = false;
            }

            /*
            if (m_Odometry->GetPosition() != getSollPosition())
            {
                Vector2D* mPosition = m_Odometry->GetPosition();

                float xDifference = mPosition->getX() - mSollPosition->getX();
                float yDifference = mPosition->getY() - mSollPosition->getY();

                // Distanz Luftline
                float distance = sqrtf(yDifference * yDifference + xDifference * xDifference);

                // Winkel zum Ziel
                float targetAngle = calculateAngle(mSollPosition->getX(), mSollPosition->getY(), mPosition->getX(), mPosition->getY());
                float remainingAngle = calculateRemainingAngle(m_Odometry->getHeading(), targetAngle);

                if (debug)
                    sLogger.debug("xDif %f, yDif %f, Distance %f", xDifference, yDifference, distance);

                // We are Further than 200mm away face our Target.
                if ((distance > 200.0) && (remainingAngle > 0.5 || remainingAngle < -0.5))
                {
                    
                    float turnSpeed = getTurnSpeed(remainingAngle / 180.0);

                    if (debug)
                        sLogger.debug("rotSpeed = %f, ActualAngle = %f, RemainingAngle = %f", turnSpeed, m_Odometry->getHeading(), remainingAngle);

                    // Roboter Drehen
                    m_Drive->Drive(0.0f, 0.0f, turnSpeed, m_Odometry->getHeading());
                    return;
                }

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
                    if ((yDifference > 5.5 || yDifference < -5.5 ))
                    {
                        setDrivingState(DRIVE_STATE_BUSY);

                        // Fahre zuerst Y Richtung
                        fSpeedX = 0.0f;
                        fSpeedY = calculateSpeed(yDifference) * -1;

                        if (debug)
                            sLogger.debug("xSpeed = %f, ySpeed = %f", fSpeedX, fSpeedY);

                        m_Drive->Drive(fSpeedX, fSpeedY, 0.0f, m_Odometry->getHeading());
                    }
                    else
                    {
                        if (xDifference > 5.5 || xDifference < -5.5)
                        {
                            setDrivingState(DRIVE_STATE_BUSY);

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

                            setDrivingState(DRIVE_STATE_FINISHED);
                            m_Drive->Drive(0.0f, 0.0f, 0.0f);
                        }
                    }
                }
            }
            else
            {
                //sLogger.debug("Position Erreicht");
                setDrivingState(DRIVE_STATE_FINISHED);
                m_Drive->Drive(0.0f, 0.0f, 0.0f);
            }
            */
        }
    }
    else
        timer -= difftime;
}

void Navigation::setSollPosition(float x, float y)
{
    mSollPosition->changeCoords(x, y);
    setDrivingState(DRIVE_STATE_BUSY);
}

float Navigation::calculateSpeed(float distance) 
{
    // Maximale Distanz (entspricht der Entfernung, bei der die Geschwindigkeit 0 erreicht)
    const float maxDistance = 200.0;

    // Berechnung der Geschwindigkeit
    float speed = (distance / maxDistance) * maxSpeed * -1;

    // Niemels unter 0.15
    if (speed != 0 && (speed < 0.05 && speed > 0))
        speed = 0.05;

    if (speed != 0 && (speed > -0.05 && speed < 0))
        speed = -0.05;

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

// Funktion zur Berechnung des Winkels zwischen dem Roboter und dem Ziel
float Navigation::calculateAngle(int targetX, int targetY, int robotX, int robotY)
{
  float dx = targetX - robotX;
  float dy = targetY - robotY;
  float angle = atan2(dy, dx) * 180 / PI;
  return angle;
}

// Funktion zur Berechnung des Winkels zwischen der aktuellen Richtung und dem Ziel
float Navigation::calculateRemainingAngle(float currentDirection, float targetDirection)
{
  float remainingAngle = targetDirection - currentDirection;
  if (remainingAngle > 180) 
  {
    remainingAngle -= 360;
  }
  else if (remainingAngle < -180)
  {
    remainingAngle += 360;
  }

  return remainingAngle;
}

float Navigation::getTurnSpeed(float turnValue) 
{
    // Begrenzung der Geschwindigkeit auf die Mindestgeschwindigkeit (+-0.5)
    if (turnValue > 0 && turnValue < 0.5) 
    {
        turnValue = 0.5;
    }
    else if (turnValue < 0 && turnValue > -0.5) 
    {
        turnValue = -0.5;
    }

    return turnValue * -1;
}

void Navigation::abortDriving()
{
    // Soll zu Istposition sollte genügen um die Motoren zu stoppen
    mSollPosition->changeCoords(m_Odometry->GetPosition()->getX(), m_Odometry->GetPosition()->getY());
}

Vector2D* Navigation::getPosition()
{
    return m_Odometry->GetPosition();
}

void Navigation::setDrivingState(DrivingState state)
{
    // Only Set new State when it Differs
    if (mDriveState != state)
    {
        mDriveState = state;
        m_Communication->sendDrivingState();
    }
}