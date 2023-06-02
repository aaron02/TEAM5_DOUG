/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

class Vector2D;
class DriveTrain;
class Odometry;
class Updateable;

enum DrivingState : uint8_t
{
    DRIVE_STATE_ERROR = 0,
    DRIVE_STATE_BUSY,
    DRIVE_STATE_FINISHED
};

class Navigation : public Updateable
{
public:
    Navigation(DriveTrain* drives, Odometry* odometry);
    ~Navigation();

    // Cyclyc Update
    void Update(uint64_t difftime) override;

    // Set target Position for Navigation process
    void setSollPosition(float x, float y);

    // get target Position
    Vector2D* getSollPosition() { return mSollPosition; }

    // get Istpositionn
    Vector2D* getPosition () { return m_Odometry->GetPosition(); }

    // set driving state
    void setDrivingState(DrivingState state) { mDriveState = state; }

    // get state of navigation process
    DrivingState getDrivingState() { return mDriveState; }

    // abort current movement
    void abortDriving();


    float calculateAngle(int targetX, int targetY, int robotX, int robotY);
    float calculateRemainingAngle(float currentDirection, float targetDirection);
    float getTurnSpeed(float turnValue);

private:
    Vector2D* mSollPosition;

    // calculates wheel speeds for Mecanum Drive dependant on Distance
    float calculateSpeed(float distance);

protected:
    DriveTrain* m_Drive;
    Odometry* m_Odometry;

    DrivingState mDriveState = DrivingState::DRIVE_STATE_FINISHED;

    float fSpeedX = 0.0f;
    float fSpeedY = 0.0f;

    int32_t timer = 50 * TimeVar::Millis;
    bool debug = false;

    // Geschwindigkeitsbereich
    const float maxSpeed = 1.0;  // Maximale Geschwindigkeit
    const float minSpeed = -1.0; // Minimale Geschwindigkeit
};