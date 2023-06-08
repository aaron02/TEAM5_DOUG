#pragma once

#include "Order.h"

enum RobotDrivingState
{
    RobotDrivingStateError,
    RobotDrivingStateBusy,
    RobotDrivingStateFinished
};

enum RobotArmState
{
    RobotArmStateError,
    RobotArmStateBusy,
    RobotArmStateFinished
};

enum RobotArmPosition
{
    RobotArmPositionReady,
    RobotArmPositionStored
};

class RobotManager
{
public:
    static bool connect(unsigned long baudRate);
    static RobotDrivingState getDrivingState();
    static RobotArmState getArmState();
    static int getBatteryState();
    static void abortDriving();
    static void setArmPosition(RobotArmPosition state);
    static void setDrivingWaypoint(Coordinates coordinates);
    static Coordinates getPosition();
    static void pickPackage(int index);
    static void placePackage(int index);

private:
    static void clearSerialInputBuffer();
    static void sendCommand(DynamicJsonDocument &doc);
    static DynamicJsonDocument getCommand(DynamicJsonDocument &doc);
};