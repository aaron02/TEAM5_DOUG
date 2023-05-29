#pragma once

#include <Arduino.h>
#include "Waypoint.h"
#include <ArduinoJson.h>

enum RobotState
{
    RobotStateError,
    RobotStateBusy,
    RobotStateFinished
};

class RobotHelper
{
public:
    RobotHelper(unsigned long baudRate);
    bool connect();
    void setNextWaypoint(Waypoint waypoint);
    bool readyForNextWaypoint();

private:
    unsigned long baudRate;
};