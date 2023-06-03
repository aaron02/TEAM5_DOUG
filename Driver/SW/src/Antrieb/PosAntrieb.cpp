/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "PosAntrieb.h"

PosAntrieb::PosAntrieb(std::string sName, uint32_t iStep,uint32_t iDir,uint32_t iEn, uint32_t iHome)
{
    sLogger.info("Controller Initialize Antrieb %s", sName.c_str());

    // Axis Initialisation
    stepper = new AccelStepper(AccelStepper::DRIVER, iStep, iDir);
    pinMode(iEn, OUTPUT);

    sAntriebName = sName;
    iStepPin = iStep;
    iDirPin = iDir;
    iEnablePin = iEn;
    iHomePin = iHome;
    
    // Default Speeds
    stepper->setMaxSpeed(500);
    stepper->setAcceleration(10);

    // DEBUG INFO
    sLogger.debug("Pins used For %s: %s", sAntriebName.c_str(), getPinsNameString().c_str());

    // Achse Referenzieren
    //setHoming();

    stepper->setSpeed(5.0);
    stepper->setAcceleration(5.0);
}

PosAntrieb::~PosAntrieb()
{
    delete stepper;
}

void PosAntrieb::Update(uint64_t difftime)
{
    // Update our Position
    if (updateCurrentPositionTimer <= 0)
    {
        updateCurrentPositionTimer = 950;
        calculateCurrentOrientation();
    }
    else
        updateCurrentPositionTimer -= difftime;

    // Move Our Axis
    if (stepper->currentPosition() != stepper->targetPosition())
        stepper->run();
}

void PosAntrieb::moveAbsolutAngle(float o, bool runHere)
{
    // Pruefen ob die eingabe zulaessig ist
    if (o < minLimitGrad || o > MaxLimitGrad)
    {
        // Debug Error
        if (debug)
        {
            sLogger.debug("Drehtisch Gradwert %f nicht zulaessig", o);
        }

        return;
    }

    long steps = long(anzahlSchritteProGrad * o);

    // Set the Absolute position to move to
    stepper->moveTo(steps);

    // Start Movement ( locked )
    if (runHere)
        stepper->runToPosition();
}

// Referenziert die Achse
void PosAntrieb::setHoming()
{
    // Initial Homing Sequenz
    // Setze Maximale Geschwindigkeit fuer den Schrittmotor fuer die Homing Sequenz
    referenziert = HOMING_IN_PROGRESS;
    stepper->setSpeed(5.0);
    stepper->setAcceleration(5.0);

    // Seriell Debug
    if (debug)
        sLogger.debug("Drehtisch Homing .......");

    // Start Homing Sequenz
    while (digitalRead(iHomePin))
    {                                   // Make the Stepper move CCW until the switch is activated   
        stepper->moveTo(initial_homing);   // Set the position to move to
        initial_homing++;               // Decrease by 1 for next move if needed
        stepper->runSpeed();                    // Start moving the stepper
        delay(1);
    }

    // Setze Positionswert auf 0
    stepper->setCurrentPosition(0);

    // Referenzpunkt erreicht fahre zurueck bis eine Flanke vom Sensor Kommt
    initial_homing = 1;

    while (!digitalRead(iHomePin))
    {
        // Make the Stepper move CW until the switch is deactivated
        stepper->moveTo(initial_homing);
        stepper->runSpeed();
        initial_homing--;
        delay(1);
    }

    // Homing Done Setze Axen Geschwindigkeit,
    // Beschleunigung sowie die Position auf Max
    if (debug)
        sLogger.debug("Homing Completed");

    // Setze Positionswert
    stepper->setCurrentPosition(long(356.5 * anzahlSchritteProGrad));
    calculateCurrentOrientation();
    
    sLogger.debug("Nullposition Greiffer Gesetzt auf %u", m_position);

    // Setze Maximale Geschwindigkeit und Beschleunigung
    stepper->setSpeed(100);
    stepper->setAcceleration(100);

    referenziert = HOMING_DONE;
}

void PosAntrieb::calculateCurrentOrientation()
{
    // Aktuelle Position / Schritte Pro Grad
    m_position = stepper->currentPosition() / anzahlSchritteProGrad;
}

std::string PosAntrieb::getPinsNameString()
{
    std::stringstream ss;

    ss << "StepPin = ";
    ss << iStepPin;
    ss << " ";
    ss << "DirPin = ";
    ss << iDirPin;
    ss << " ";
    ss << "EnablePin = ";
    ss << iEnablePin;
    ss << " ";
    ss << "HomePin = ";
    ss << iHomePin;

    return ss.str();
}