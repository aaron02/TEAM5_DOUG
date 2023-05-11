/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

enum Status
{
    Startup = 0,
    Initialization,
    Started
};

// Arduino
#include <Arduino.h>

// Libaries
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Adafruit_BNO08x.h>
#include <Wire.h>
#include "SPI.h"

// STD LIBS
#include <stdio.h>
#include <chrono>
#include <iostream>
#include <string>
#include <cstdarg>
#include <sstream> 
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <vector>
#include <array>
#include <cassert>
#include <string_view>
#include "estd/span.h"

// User Created
#include "Logger.h"
#include "TimeVar.h"
#include "Antrieb/Antrieb.h"
#include "Navigation.h"
#include "Antrieb/DriveTrain.h"
#include "Odometry.h"
#include "Vector2D.h"
#include "Sensors/ADNS.h"
#include "Sensors/Gyro.h"
#include "Sensors/PowerDistributionBoard.h"
