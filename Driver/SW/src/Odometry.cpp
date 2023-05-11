/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "Odometry.h"

Odometry::Odometry(Gyro* nav, ADNS_CTRL* ctrl) : gyro(nav), adnsController(ctrl)
{
    Initialize();
}

Odometry::~Odometry()
{

}

void Odometry::Initialize()
{
    position = new Vector2D(0, 0);
    dStartHeading = 0;
    dHeading = normalizeRadians(dHeading + dStartHeading);
}

void Odometry::setStartLocation(Vector2D startPosition, double startHeading)
{ 
    position = new Vector2D(startPosition);
    dStartHeading = radians(startHeading * -1);
    dHeading = normalizeRadians(dHeading + dStartHeading);
}


void Odometry::Update(uint64_t difftime)
{
    // Update Sensor evry 10 ms
    if (sensorUpdate <= 0)
    {
        CalculatePosition(adnsController->getXDistance(), adnsController->getYDistance());
        adnsController->ResetXYDistance();
        sensorUpdate = 10 * TimeVar::Millis;;
    }
    else
        sensorUpdate = sensorUpdate - difftime;

    // Output Info evry 1 s
    if (debug)
    {
        if (timer < 0)
        {
            //CalculatePosition(32.283464566929133858267716535433, 64.566929133858267716535433070866);
            sLogger.info("PositionX: %f PositionY: %f Heading = %f", position->getX(), position->getY(), degrees(dHeading));

            timer = 2 * TimeVar::Seconds;
        }
        else
            timer = timer - difftime;
    }
}

void Odometry::CalculatePosition(double x, double y)
{
    // counts per inch -> 1 inch = 2.54 cm
    dDeltax = x / 8200 * 254;
    dDeltay = y / 8200 * 254;

    double dGyroAngle = radians(gyro->getGyroAngle(GYRO_AXIS::YAW) + degrees(dStartHeading) + dHeadingCorrection);

    dHeading = normalizeRadians(dGyroAngle);

    Vector2D temp = Vector2D(dDeltax, dDeltay);
    temp.rotate(dHeading);

    position->add(&temp);
}

double Odometry::normalizeRadians(double angle)
{
    while (angle >= 2 * PI) 
    {
        angle -= 2 * PI;
    }

    while (angle < 0.0) 
    {
        angle += 2 * PI;
    }

    return angle;
}