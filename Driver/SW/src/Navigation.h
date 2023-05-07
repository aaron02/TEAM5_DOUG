/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Defnies.h"

class DriveTrain;
class Odometry;

class Navigation
{
public:
    Navigation(DriveTrain* drives, Odometry* odometry);
    ~Navigation();

    void Update(uint64_t difftime);

private:
    int32_t moveTimer = 5 * TimeVar::Seconds;
    uint8_t moveStep = 0;

protected:
    DriveTrain* m_Drive;
    Odometry* m_Odometry;
};