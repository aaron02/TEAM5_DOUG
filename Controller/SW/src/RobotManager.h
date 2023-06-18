#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Coordinates.h"
#include "Log.h"

// Different states of the robot
enum RobotDrivingState
{
    RobotDrivingStateUndefined,
    RobotDrivingStateError,
    RobotDrivingStateBusy,
    RobotDrivingStateReady
};

// Different states of the robot arm
enum RobotArmState
{
    RobotArmStateUndefined,
    RobotArmStateError,
    RobotArmStateBusy,
    RobotArmStateReady
};

// This class is used to manage the robot
class RobotManager
{
public:
    /// @brief Initialize the communication
    /// @param serialBaud Baud rate of the serial connection
    /// @param serialTimeout_ms Timeout for the serial connection in milliseconds
    /// @return True if the initialization was successful otherwise false
    static bool initialize(unsigned long serialBaud, unsigned long serialTimeout_ms);

    /// @brief Start driving to a new waypoint and set the driving state to busy
    static void startDrivingToWaypoint(Coordinates coordinates);

    /// @brief Stop driving immediately
    static void stopDriving();

    /// @brief Start picking a package and set the arm state to busy
    /// @param roboStorageIndex The index of the storage on the robot
    /// @param distributionCenterIndex The index of the distribution center
    static void startPickPackage(int robotStorageIndex, int distributionCenterIndex);

    /// @brief Start placing a package and set the arm state to busy
    static void startPlacePackage(int robotStorageIndex);

    /// @brief Process incoming messages and update all states
    static void processIncomingeMessages();

    /// @brief  Clear all incoming messages
    static void clearIncomingeMessages();

    /// @brief Get the battery state in percent
    static int getBatteryState();

    /// @brief Get the current position
    static Coordinates getCurrentPosition();

    /// @brief  Get the current driving state
    static RobotDrivingState getDrivingState();

    /// @brief  Get the current arm state
    static RobotArmState getArmState();

private:
    /// @brief Battery state in percent
    static int currentBatteryState;

    /// @brief Current position
    static Coordinates currentPosition;

    /// @brief Current driving state
    static RobotDrivingState drivingState;

    /// @brief  Current arm state
    static RobotArmState armState;

    /// @brief Send a command to the robot
    /// @param payload Payload in JSON format
    static void sendCommand(DynamicJsonDocument &payload);

    /// @brief Request arm state
    static void requestArmState();
};