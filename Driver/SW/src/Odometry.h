/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

class Vector2D;
class Gyro;
class ADNS_CTRL;

class Odometry
{
public:
    Odometry(Gyro* gyro, ADNS_CTRL* adns);
    ~Odometry();

    void Initialize();
    void setStartLocation(Vector2D startPosition, double startHeading);
    double normalizeRadians(double angle);

    void Update(uint64_t difftime);
    void CalculatePosition(double x, double y);

    Vector2D* GetPosition() { return position; }

    Gyro* getGyro() { return gyro; }

    // Aktuelle Richtung
    double dHeading = 0;

    // Richtung zum Start der Messung
    double dStartHeading = 0.0f;

    // Deltas
    double dDeltax;
    double dDeltay;

    // Korrekturfaktor
    double dHeadingCorrection = 90.0f;

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
