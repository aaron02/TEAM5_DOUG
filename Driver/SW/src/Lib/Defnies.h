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

enum HomingStatus
{
    HOMING_NOT_DONE         = 0,
    HOMING_IN_PROGRESS      = 1,
    HOMING_DONE             = 2
};

enum MotorType 
{
    kFrontLeft              = 0,
    kFrontRight             = 1,
    kRearLeft               = 2,
    kRearRight              = 3
};

#define CALL_UPDATE(obj, func) if (obj) static_cast<Updateable*>(obj)->func

// Arduino
#include <Arduino.h>

// Libaries
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Adafruit_BNO08x.h>
#include <Wire.h>
#include "SPI.h"
#include <ArduinoJson.h>
#include <SD.h>
#include <PWMServo.h>
#include <TCA9548.h>
#include <M5Stack.h>

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
#include <map>
#include <unordered_map>

// User Created
#include "Updateable.h"
#include "Logger.h"
#include "TimeVar.h"
#include "Vector2D.h"
#include "Opcodes.hpp"
#include "Antrieb/Antrieb.h"
#include "Antrieb/Antrieb2.h"
#include "Antrieb/PosAntrieb.h"
#include "Antrieb/DriveTrain.h"
#include "Management/Navigation.h"
#include "Management/Odometry.h"
#include "Management/Com.h"
#include "Sensors/ADNS.h"
#include "Sensors/Gyro.h"
#include "Sensors/PowerDistributionBoard.h"
#include "Greifer/Greifer.h"
#include "Greifer/i2cTOF.h"