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
    Communication(Navigation* mNavigation ,Greifer* mGreifer, PDB* mPower);
    ~Communication();

    void Update(uint64_t difftime);

private:

    uint8_t getFunctionIndex(std::string command);

    void response(std::string response, std::string message = "");
    void responseData(std::string response, std::string message);

protected:
    sFunctions functionHandle;

    // Handles
    Navigation* mNavigation = nullptr;
    Greifer* mGreifer = nullptr;
    PDB* mPower = nullptr;
};