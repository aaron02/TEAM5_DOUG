/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

#include <map>
#include <unordered_map>

class Greifer;
class Navigation;
class PDB;
class Odometry;

enum Functions : uint8_t
{
    Unk = 0,
    GetCurrentPosition,
    SetDrivingWaypoint,
    AbortDriving,
    GetDrivingState,
    SetArmState,
    GetArmState,
    PickPackage,
    PlacePackage,
    GetBatteryState,
    MAXFUNCTIONS
};

typedef std::unordered_map<std::string, Functions> sFunctions;

class Communication
{
public:
    Communication(Navigation* mNavigation ,Greifer* mGreifer, PDB* mPower, Odometry* mOdo);
    ~Communication();

    // Cyclyc Update
    void Update(uint64_t difftime);

private:

    // returns a function indes of our enum Functions
    uint8_t getFunctionIndex(std::string command);

    // sends a Response to the controller
    void response(std::string response, std::string message = "");

    // send a single data to the controller
    void responseData(std::string response, std::string message);

protected:
    sFunctions functionHandle;

    // Handles
    Navigation* mNavigation = nullptr;
    Odometry* mOdometry = nullptr;
    Greifer* mGreifer = nullptr;
    PDB* mPower = nullptr;
};