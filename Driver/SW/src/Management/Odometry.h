/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

class Vector2D;
class Gyro;
class ADNS_CTRL;
class Updateable;

class Odometry : public Updateable
{
public:
    Odometry(Gyro* gyro, ADNS_CTRL* adns);
    ~Odometry();

    // Load up all neccesary stuff
    void Initialize();
    
    // Set our Start Position
    void setStartLocation(Vector2D startPosition, double startHeading);
    void setLocation(Vector2D startPosition);

    // Normalize Radians
    double normalizeRadians(double angle);

    // Cyclyc Update
    void Update(uint64_t difftime) override;

    // Calculates Actual Position
    void CalculatePosition(double x, double y);

    Vector2D* GetPosition() { return position; }
    float getHeading() { return degrees(dHeading) - dHeadingCorrection; }

    Gyro* getGyro() { return gyro; }

    // Aktuelle Richtung
    double dHeading = 0;

    // Richtung zum Start der Messung
    double dStartHeading = 0.0f;

    // Deltas
    double dDeltax;
    double dDeltay;

    // Korrekturfaktor
    double dHeadingCorrection = 1.80491f;

    // Aktuelle Position
    Vector2D* position;
    Vector2D* fieldCentricDelta;
    Vector2D* robotCentricDelta;

private:
    int32_t sensorUpdate = 100 * TimeVar::Millis;
    int32_t timer = 1 * TimeVar::Seconds;

protected:
    Gyro* gyro;
    ADNS_CTRL* adnsController;

    bool debug = true;
};
