#include "RobotHelper.h"

RobotHelper::RobotHelper(unsigned long baudRate) : baudRate(baudRate)
{
}

bool RobotHelper::connect()
{
    // Init port
    Serial1.begin(baudRate, SERIAL_8N1, 18, 17);

    // Check if port is ready
    if (!Serial1)
    {
        return false;
    }
    Serial1.println("hello");
    return true;
}

void RobotHelper::setNextWaypoint(Waypoint waypoint)
{
    DynamicJsonDocument sendDoc(1024);
    sendDoc["Command"] = "SetNextWaypoint";
    sendDoc["Data"]["x"] = waypoint.GetCoordinates().x;
    sendDoc["Data"]["y"] = waypoint.GetCoordinates().y;

    serializeJson(sendDoc, Serial1);
}

bool RobotHelper::readyForNextWaypoint()
{
    DynamicJsonDocument sendDoc(1024);
    DynamicJsonDocument recieveDoc(1024);

    RobotState state = RobotStateError;

    sendDoc["Command"] = "GetDrivingState";
    while (Serial1.available())
    {
        Serial1.read();
    }

    serializeJson(sendDoc, Serial1);

    while (!Serial1.available())
    {
    }

    deserializeJson(recieveDoc, Serial1);

    String returnedstate = recieveDoc["Response"];
    Serial.println(returnedstate);
    if (returnedstate == "Finished")
    {
        state = RobotState::RobotStateFinished;
    }
    else if (returnedstate == "Busy")
    {
        state = RobotState::RobotStateBusy;
    }

    if (state == RobotState::RobotStateFinished)
    {
        return true;
    }

    return false;
}

bool RobotHelper::addWaypointToQueue(Waypoint waypoint)
{
    return waypointQueue.enqueue(waypoint);
}

Waypoint RobotHelper::popWaypointFromQueue()
{
    return waypointQueue.dequeue();
}

bool RobotHelper::hasWaypointInQueue()
{
    return !waypointQueue.isEmpty();
}
