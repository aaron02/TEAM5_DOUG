/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

class Greifer;
class Navigation;
class PDB;
class Odometry;
class Updateable;
class Communication;

struct OpcodeHandler
{
    void (Communication::*handler)(JsonDocument& doc);
};

class Communication : public Updateable
{
public:
    Communication(Navigation* mNavigation ,Greifer* mGreifer, PDB* mPower, Odometry* mOdo);
    ~Communication();

    // Cyclyc Update
    void Update(uint64_t difftime) override;

    static void loadHandlers();

    // Helpers
    void sendBatteryState();
    void sendCurrentPosition();
    void sendDrivingState();
    void sendArmState();

private:

    // returns a function indes of our enum Functions
    uint8_t getFunctionIndex(std::string command);

    // Debug Outputs enabled
    bool debug = false;

protected:
    // Handles
    Navigation* mNavigation = nullptr;
    Greifer* mGreifer = nullptr;
    PDB* mPower = nullptr;
    Odometry* mOdometry = nullptr;

    // Timer
    int32_t timer = 1 * TimeVar::Seconds;

    // Handlers
    void handleGetCurrentPosition(JsonDocument& doc);
    void handleSetDrivingWaypoint(JsonDocument& doc);
    void handleAbortDriving(JsonDocument& doc);
    void handleGetDrivingState(JsonDocument& doc);
    void handleSetArmStatus(JsonDocument& doc);
    void handleGetArmStatus(JsonDocument& doc);
    void handlePickPackage(JsonDocument& doc);
    void handlePlacePackage(JsonDocument& doc);
    void handleGetBatteryState(JsonDocument& doc);
    void handleCustomerAccepted(JsonDocument& doc);
};