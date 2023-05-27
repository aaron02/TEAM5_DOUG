/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

#include <map>
#include <unordered_map>

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
    Communication();
    ~Communication();

    void Update(uint64_t difftime);

private:

    uint8_t getFunctionIndex(std::string command);

protected:
    sFunctions functionHandle;
};