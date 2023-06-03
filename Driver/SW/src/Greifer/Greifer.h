/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

class PosAntrieb;
class Navigation;
class Updateable;
class Odometry;

enum ServoMapping
{
    SERVO_BASE = 0,
    SERVO_GRIPP
};

enum ArmStatus
{
    AS_Undefined = 0,
    AS_Grundstellung,
    AS_PickPackage,
    AS_PlacePackage,
    AS_Ready,
    AS_Error
};

enum PackStatus
{
    STATUS_FAILED = 0,
    STATUS_Undefined,
    STATUS_ReadyToMove,
    STATUS_WaitingForCustomer,
    STATUS_OK
};

enum SERVO_Positionen
{
    //SERVO_BASE
    BASE_GS = 90, //GS=Grundstellung
    BASE_OBEN = 60,
    BASE_GEIFERPOSITION = 126,
    BASE_PARKPOSITION = 90,
    BASE_HOVEROVERLAGER = 81,
    BASE_LAGERPOSITION = 99, 
    BASE_LAGERPOSITIONAUFNAHME = 99, //backup falls aufnahme und ablage unterschiedlich sein müssen.
    //SERVO_GRIPP
    GRIPP_OFFEN = 60,
    GRIPP_GESCHLOSSEN = 15,
    GRIPP_PARKPOSITION = 60,
    GRIPP_OPEN_LAGER = 35
};

enum Drehtisch_Position
{
    DT_Lager_PS1 = 205,
    DT_Lager_PS2 = 187,
    DT_Lager_PS3 = 169,
    DT_Lager_PS4 = 151,
    DT_Parkposition = 180, // Soll in der mitte vom Robo sein
    DT_Greifposition = 360,
    DT_Bewegung_Freigegeben = 270  //Greifer-Arm hat sich bis zu dieser Position bewegt wo er nicht mehr im Vorderen
                                   //Teil des Roboters ist. Damit könnte man ein Signal ausgeben das sich der Roboter bewegen darf. 
};

class Greifer : public Updateable
{
public:
    Greifer(uint8_t servoPin1, uint8_t servoPin2, PosAntrieb& dreh, Navigation& nav);
    ~Greifer();

    void Update(uint64_t difftime) override;
    uint8_t getPosition(ServoMapping servo);
    uint8_t getSollPosition(ServoMapping servo);
    void setSollPosition(ServoMapping servo, SERVO_Positionen degree); // uint8_t degree

    // Drive Befehle
    void Grundstellung();
    void PickPackage();
    void PlacePackage();

    void setArmStatus(ArmStatus state) { iGripperState = state;}
    ArmStatus getArmStatus() { return iGripperState; }
    void setPackStatus(PackStatus Status) { iPackStatus = Status; }
    PackStatus getPackStatus() { return iPackStatus; }

    // Index für die Automatikfahr
    void setLagerIndex(uint8_t index) { iLagerindex = index; }

    Drehtisch_Position getPositionFromIndex(uint8_t index);

    void setTimer(ServoMapping servo, int32_t timer);

    // Schnittstelle Kunde
    bool PaketAnnahmeBestätigungKunde = false; //Freigabe das der Kunde berechtigt ist das paket zu empfangen.
    bool PaketKundeOderAblageort = false; // Ablageart definieren 0=Kunde 1=Ablageort

private:
    void runServo(ServoMapping servoIndex, uint64_t difftime);
    std::vector<int32_t> updateTimer = { 1 * TimeVar::Seconds, 1 * TimeVar::Seconds };
    bool inposition();

protected:
    PWMServo servo[2];

    long servoPos[2];

    /*
        INFORMATION FÜR MICH
        Schrittmotor 1 Step = 1.8° -> 200 Steps = 360°
        Driver -> Set to 1/8 Steps  -> 200*8 = 1600 steps = 360°

    mAntrieb->setSpeeds(300, 360);
    */

    uint8_t iGrundstellungStep = 0;
    uint8_t iPickPackageStep = 0;
    uint8_t iPlacePackageStep = 0;
    ArmStatus iGripperState = ArmStatus::AS_Undefined;
    PackStatus iPackStatus = PackStatus::STATUS_Undefined;
    uint8_t iLagerindex = 0;

    PosAntrieb* mAntrieb = nullptr;
    Navigation* mNavigation = nullptr;
    uint8_t servo1PositionToGo = 0;
    uint8_t servo2PositionToGo = 0;
};