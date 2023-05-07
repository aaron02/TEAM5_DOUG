/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include <Arduino.h>

enum TimeVar : uint64_t 
{
    Mikro   = 1000,
    Millis  = 1 * Mikro,
    Seconds = 1000 * Millis,
    Minutes = 60 * Millis,
    Hours   = 60 * 60 * Millis
};