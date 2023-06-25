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

void Odometry::setLocation(Vector2D startPosition)
{
    position->changeCoords(startPosition.getX(), startPosition.getY());
}

void Odometry::Update(uint64_t difftime)
{
    // Update Sensor evry 10 ms
    if (sensorUpdate <= 0)
    {
        CalculatePosition(adnsController->getYDistance() * -1, adnsController->getXDistance() * -1);
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
            // Constant Speed Test
            //CalculatePosition(32.283464566929133858267716535433, 64.566929133858267716535433070866);
            sLogger.info("PositionX: %f PositionY: %f Heading = %f", position->getX(), position->getY(), getHeading());
            // Distance traveled Test
            //setStartLocation(Vector2D(0, 0), 0);
            timer = 5 * TimeVar::Seconds;
        }
        else
            timer = timer - difftime;
    }
}

void Odometry::CalculatePosition(double x, double y)
{
    // counts per inch -> 1 inch = 2.54 cm / 2540
    dDeltax = x / 8200.0f * 25.4f;
    dDeltay = y / 8200.0f * 25.4f;


    // Gyro Runden
    float gyroRounded = float( (round(gyro->getGyroAngle(GYRO_AXIS::YAW) * 10.0f)) / 10.0f );

    double dGyroAngle = radians(gyroRounded + degrees(dStartHeading) + dHeadingCorrection);

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