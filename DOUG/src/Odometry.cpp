/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "Odometry.h"
#include "ADNS.h"

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
    dStartHeading = DEG_TO_RAD * startHeading;
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
    if (timer < 0)
    {
        //sLogger.info("%f",gyro->getGyroAngle(GYRO_AXIS::YAW));
        //sLogger.info("PositionX: %f PositionY: %f Heading = %f", position->getX(), position->getY(), 0);

        timer = 2 * TimeVar::Seconds;
    }
    else
        timer = timer - difftime;
}

void Odometry::CalculatePosition(double x, double y)
{
    dDeltax = x / 8200 * 254;
    dDeltay = y / 8200 * 254;
    double dGyroAngle = gyro->getGyroAngle(GYRO_AXIS::YAW);

    dHeading = normalizeRadians(dGyroAngle + dStartHeading + dHeadingCorrection);

    Vector2D temp = Vector2D(dDeltax, dDeltay);
    //temp.rotate(dHeading);

    position->add(&temp);

    //robotCentricDelta = new Vector2D((dDeltax), ( dDeltay));
    //fieldCentricDelta = new Vector2D((dDeltay), (-dDeltax));

    // Rotate our Vector by the Gyro Angle
    //fieldCentricDelta->rotate(dHeading);
    //sLogger.info("%f", position->getX());
    //position->add(fieldCentricDelta);
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