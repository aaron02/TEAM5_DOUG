/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

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
    CustomerAccepted,
    MAXFUNCTIONS
};

static std::unordered_map<std::string, Functions> mOpcodes =
{
    {"GetCurrentPosition", GetCurrentPosition},
    {"SetNextWaypoint", SetDrivingWaypoint},
    {"AbortDriving", AbortDriving},
    {"GetDrivingState", GetDrivingState},
    {"SetArmState", SetArmState},
    {"GetArmState", GetArmState},
    {"PickPackage", PickPackage},
    {"PlacePackage", PlacePackage},
    {"GetBatteryState", GetBatteryState},
};