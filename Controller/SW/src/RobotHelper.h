#pragma once

#include <Arduino.h>
#include "Waypoint.h"
#include <ArduinoJson.h>
#include <ArduinoQueue.h>

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
    bool addWaypointToQueue(Waypoint waypoint);
    Waypoint popWaypointFromQueue();
    bool hasWaypointInQueue();

private:
    unsigned long baudRate;
    ArduinoQueue<Waypoint> waypointQueue;
};