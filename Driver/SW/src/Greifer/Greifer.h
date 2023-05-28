/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

class PosAntrieb;
class Navigation;

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
    STATUS_FAILED = 0,
    STATUS_OK
};

enum Grundstellung
{
    GS_Running = 0,
    GS_OK
};

enum SERVO_Positionen
{
    //SERVO_BASE
    BASE_GS = 130, //GS=Grundstellung
    BASE_OBEN = 160,
    BASE_GEIFERPOSITION = 60,
    BASE_PARKPOSITION = 90,
    BASE_HOVEROVERLAGER = 120,
    BASE_LAGERPOSITION = 110,
    //SERVO_GRIPP
    GRIPP_OFFEN = 160,
    GRIPP_GESCHLOSSEN = 60,
    GRIPP_PARKPOSITION = 90,
};

enum Drehtisch_Position
{
    DT_Lager_PS1 =1,
    DT_Lager_PS2,
    DT_Lager_PS3,
    DT_Lager_PS4,
    DT_Parkposition = 45, // Soll in der mitte vom Robo sein
    DT_Greifposition = 270,
    DT_Bewegung_Freigegeben = 180, //Greifer-Arm hat sich bis zu dieser Position bewegt wo er nicht mehr im Vorderen
                                   //Teil des Roboters ist. Damit könnte man ein Signal ausgeben das sich der Roboter bewegen darf. 
};

class Greifer
{
public:
    Greifer(uint8_t servoPin1, uint8_t servoPin2, PosAntrieb& dreh, Navigation& nav);
    ~Greifer();

    void Update(uint64_t difftime);

    uint8_t getPosition(ServoMapping servo);
    uint8_t getSollPosition(ServoMapping servo);
    void setSollPosition(ServoMapping servo, SERVO_Positionen degree); // uint8_t degree

    // Drive Befehle
    
    Grundstellung Grundstellung();
    void setArmStatus();
    ArmStatus getArmStatus();
    PackStatus PickPackage(uint8_t lagerIndex);
    PackStatus PlacePackage(uint8_t lagerIndex);

    void setTimer(ServoMapping servo, int32_t timer);

private:
    void runServo(ServoMapping servoIndex, uint64_t difftime);
    std::vector<int32_t> updateTimer = { 500, 500 };

protected:
    PWMServo servo[2];

    /*
        Schrittmotor 1 Step = 1.8° -> 200 Steps = 360°
        Driver -> Set to 1/8 Steps  -> 200*8 = 1600 steps = 360°

    mAntrieb->setSpeeds(300, 360);
    */
    PosAntrieb* mAntrieb = nullptr;
    Navigation* mNavigation = nullptr;
    uint8_t servo1PositionToGo = 0;
    uint8_t servo2PositionToGo = 0;
};