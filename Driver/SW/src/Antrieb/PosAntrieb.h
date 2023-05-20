/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Lib/Defnies.h"

class PosAntrieb
{
    public:
    PosAntrieb(std::string sName, uint32_t iStep, uint32_t iDir, uint32_t iEn, uint32_t iHome);
    ~PosAntrieb();

    void Update(uint64_t difftime);

    // Speed Functions
    float getPosition() { return m_position; }
    bool isHomed() { return referenziert == HOMING_DONE ? true : false; }
    bool isMoving() { return stepper->isRunning(); }
    bool inPosition() { return stepper->distanceToGo() == 0; }

    // Bewegt die Achse in Absoluter Ebene
    void moveAbsolutAngle(float o, bool runHere = false);

    // Set Speed and Acceleration
    void setSpeeds(float accel, float speed) { stepper->setAcceleration(accel); stepper->setSpeed(speed); };

    // Referenziert die Achse
    void setHoming();

    std::string getPinsNameString();

    private:
    // Stepper Axis
    AccelStepper* stepper;
    float m_position;
    HomingStatus referenziert = HOMING_NOT_DONE;

    // Berechne Winkel der Achse
    void calculateCurrentOrientation();

    // Software Limits
    int32_t minLimitGrad = 0;
    int32_t MaxLimitGrad = 360;

    // Anzahl Schritte für 1 Umdrehung der Achse
    float anzahlSchritteProGrad = 13.33;
    long anzahlSchritteMax = 4800;

    // Timer for Orientation Updates
    int32_t updateCurrentPositionTimer = 950;

    // Homing Position
    long initial_homing = -1;

    // Debug flag
    bool debug = true;

    std::string sAntriebName = "";
    uint32_t iStepPin = 0;
    uint32_t iDirPin = 0;
    uint32_t iEnablePin = 0;
    uint32_t iHomePin = 0;
};