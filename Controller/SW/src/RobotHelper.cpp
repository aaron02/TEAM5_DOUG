#include "RobotHelper.h"

RobotHelper::RobotHelper(HardwareSerial &serial, unsigned long baudRate) : serial(serial), baudRate(baudRate)
{
}

bool RobotHelper::connect()
{
    // Init port
    serial.begin(baudRate);

    // Check if port is ready
    if (!serial)
    {
        return false;
    }
serial.println("hello");
    return true;
}

void RobotHelper::setNextWaypoint(Waypoint waypoint)
{
    DynamicJsonDocument sendDoc(1024);
    sendDoc["Command"] = "SetNextWaypoint";
    sendDoc["Data"]["x"] = waypoint.GetCoordinates().x;
    sendDoc["Data"]["y"] = waypoint.GetCoordinates().y;

    serializeJson(sendDoc, serial);
    serial.println();
}

bool RobotHelper::readyForNextWaypoint()
{
    DynamicJsonDocument sendDoc(1024);
    DynamicJsonDocument recieveDoc(1024);

    RobotState state = RobotStateError;

    sendDoc["Command"] = "GetDrivingState";
    serializeJson(sendDoc, serial);
    serial.println();

    deserializeJson(recieveDoc, serial);

    String returnedstate = recieveDoc["Data"]["State"];

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
