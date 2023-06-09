#include "RobotManager.h"
#include <ArduinoJson.h>

bool RobotManager::connect(unsigned long baudRate)
{
    // Init port
    Serial1.begin(baudRate, SERIAL_8N1, 18, 17);

    // Check if port is ready
    if (!Serial1)
    {
        return false;
    }

    return true;
}

RobotDrivingState RobotManager::getDrivingState()
{
    DynamicJsonDocument sendDoc(1024);
    sendDoc["Command"] = "GetDrivingState";
    DynamicJsonDocument recieveDoc = getCommand(sendDoc);

    String returnedstate = recieveDoc["Response"];

    if (returnedstate == "Finished")
    {
        Log::println(LogType::LOG_TYPE_LOG, "State: Finished");
        return RobotDrivingState::RobotDrivingStateFinished;
    }
    else if (returnedstate == "Busy")
    {
        Log::println(LogType::LOG_TYPE_LOG, "State: Busy");
        return RobotDrivingState::RobotDrivingStateBusy;
    }

    return RobotDrivingState::RobotDrivingStateError;
}

RobotArmState RobotManager::getArmState()
{
    DynamicJsonDocument sendDoc(1024);
    sendDoc["Command"] = "GetArmState";
    DynamicJsonDocument recieveDoc = getCommand(sendDoc);

    String returnedstate = recieveDoc["Response"];

    if (returnedstate == "AS_Undefined")
    {
        return RobotArmState ::RobotArmStateError;
    }
    else if (returnedstate == "AS_Grundstellung")
    {
        return RobotArmState ::RobotArmStateFinished;
    }
    else if (returnedstate == "AS_PickPackage")
    {
        return RobotArmState ::RobotArmStateBusy;
    }
    else if (returnedstate == "AS_PlacePackage")
    {
        return RobotArmState ::RobotArmStateBusy;
    }
    else if (returnedstate == "AS_Ready")
    {
        return RobotArmState ::RobotArmStateFinished;
    }
    else if (returnedstate == "AS_Error")
    {
        return RobotArmState ::RobotArmStateError;
    }

    return RobotArmState ::RobotArmStateError;
}

int RobotManager::getBatteryState()
{
    DynamicJsonDocument sendDoc(1024);
    sendDoc["Command"] = "GetBatteryState";
    DynamicJsonDocument recieveDoc = getCommand(sendDoc);

    return recieveDoc["BatteryState"].as<int>();
}

void RobotManager::abortDriving()
{
    DynamicJsonDocument sendDoc(1024);
    sendDoc["Command"] = "AbortDriving";
    sendCommand(sendDoc);
}

void RobotManager::setArmPosition(RobotArmPosition state)
{
    DynamicJsonDocument sendDoc(1024);
    sendDoc["Command"] = "SetArmState";
    switch (state)
    {
    case RobotArmPosition::RobotArmPositionReady:
        sendDoc["Data"]["ArmState"] = "Ready";
        break;
    case RobotArmPosition::RobotArmPositionStored:
        sendDoc["Data"]["ArmState"] = "Stored";
        break;
    default:
        break;
    }
    sendCommand(sendDoc);
}

void RobotManager::setDrivingWaypoint(Coordinates coordinates)
{
    DynamicJsonDocument sendDoc(1024);
    sendDoc["Command"] = "SetNextWaypoint";
    sendDoc["Data"]["x"] = coordinates.x;
    sendDoc["Data"]["y"] = coordinates.y;
    sendCommand(sendDoc);
}

Coordinates RobotManager::getPosition()
{
    DynamicJsonDocument sendDoc(1024);
    sendDoc["Command"] = "GetCurrentPosition";
    DynamicJsonDocument recieveDoc = getCommand(sendDoc);

    Coordinates coordinates;
    coordinates.x = recieveDoc["Data"]["x"].as<int>();
    coordinates.y = recieveDoc["Data"]["y"].as<int>();

    return coordinates;
}

void RobotManager::pickPackage(int index)
{
    DynamicJsonDocument sendDoc(1024);
    sendDoc["Command"] = "PickPackage";
    sendDoc["Data"]["Lagerindex"] = index;
    sendDoc["Data"]["Autonom"] = true;
    sendCommand(sendDoc);
}

void RobotManager::placePackage(int index)
{
    DynamicJsonDocument sendDoc(1024);
    sendDoc["Command"] = "PlacePackage";
    sendDoc["Data"]["Lagerindex"] = index;
    sendCommand(sendDoc);
}

void RobotManager::clearSerialInputBuffer()
{
    while (Serial1.available())
    {
        char c = Serial1.read();
    }
}

void RobotManager::sendCommand(DynamicJsonDocument &doc)
{

    serializeJson(doc, Serial1);
}

DynamicJsonDocument RobotManager::getCommand(DynamicJsonDocument &doc)
{
    Serial.setTimeout(2000);

    clearSerialInputBuffer();

    sendCommand(doc);

    DynamicJsonDocument recieveDoc(1024);
    deserializeJson(recieveDoc, Serial1);

    return recieveDoc;
}