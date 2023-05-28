/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

class Vector2D;
class DriveTrain;
class Odometry;

class Navigation
{
public:
    Navigation(DriveTrain* drives, Odometry* odometry);
    ~Navigation();

    void Update(uint64_t difftime);

    // Setze Sollposition für Navigation
    void setSollPosition(float x, float y);
    Vector2D* getSollPosition() { return mSollPosition; }

    float calculateSpeed(int distance);

private:
    Vector2D* mSollPosition;

protected:
    DriveTrain* m_Drive;
    Odometry* m_Odometry;

    float fSpeedX = 0.0f;
    float fSpeedY = 0.0f;

    int32_t timer = 1 * TimeVar::Seconds;
    bool debug = false;

    // Geschwindigkeitsbereich
    const float maxSpeed = 1.0;  // Maximale Geschwindigkeit
    const float minSpeed = -1.0; // Minimale Geschwindigkeit
};