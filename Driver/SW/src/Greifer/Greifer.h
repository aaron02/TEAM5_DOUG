/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

#include <PWMServo.h>

class PosAntrieb;

enum ServoMapping
{
    SERVO_BASE = 0,
    SERVO_GRIPP
};

enum ArmStatus
{
    ERROR = 0,
    READY,
    STORED
};

enum PackStatus
{
    FAILED = 0,
    OK
};

PWMServo servo[2];

class Greifer
{
public:
    Greifer(uint8_t servoPin1, uint8_t servoPin2, PosAntrieb& dreh, Navigation& nav);
    ~Greifer();

    void Update(uint64_t difftime);

    uint8_t getPosition(ServoMapping servo);
    uint8_t getSollPosition(ServoMapping servo);
    void setSollPosition(ServoMapping servo, uint8_t degree);

    // Drive Befehle
    void setArmStatus();
    ArmStatus getArmStatus();
    PackStatus PickPackage(uint8_t lagerIndex);
    PackStatus PlacePackage(uint8_t lagerIndex);

    void setTimer(ServoMapping servo, int32_t timer);

private:
    void runServo(ServoMapping servoIndex, uint64_t difftime);
    std::vector<int32_t> updateTimer = { 500, 500 };

protected:
    PosAntrieb* mAntrieb = nullptr;
    Navigation* mNavigation = nullptr;
    uint8_t servo1PositionToGo = 0;
    uint8_t servo2PositionToGo = 0;

    // Lagerkonstanten
    const uint8_t iPlace1 = 25;
    const uint8_t iPlace2 = 50;
    const uint8_t iPlace3 = 75;
    const uint8_t iPlace4 = 100;
};