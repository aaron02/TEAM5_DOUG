/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Defnies.h"

class Antrieb;

struct WheelSpeeds 
{
    double frontLeft    = 0.0;
    double frontRight   = 0.0;
    double rearLeft     = 0.0;
    double rearRight    = 0.0;
};

enum MotorType 
{
    kFrontLeft          = 0,
    kFrontRight         = 1,
    kRearLeft           = 2,
    kRearRight          = 3
};

class DriveTrain
{
    public:
    DriveTrain(Antrieb& frontLeftMotor, Antrieb& rearLeftMotor, Antrieb& frontRightMotor, Antrieb& rearRightMotor);
    ~DriveTrain();

    /*
        Applies the Deadband to the input Value
    */
    double ApplyDeadband(double value, double deadband, double maxMagnitude);

    /*
        Inputs smaller than the Deadband are set to 0.0, Larger Values are Scaled from 0.0 to 1.0
    */
    void SetDeadband(double deadband);

    /*
        Scaling Factor for Motor Controllers
    */
    void SetMaxOutput(double maxOutput);

    /*
        xSpeed speed along the X axis [-1.0..1.0].
        ySpeed speed along the Y axis [-1.0..1.0].
        zRotation rotation rate around the Z Axis [-1.0..1.0].
        gyroAngle rotation around the Z Axis.
    */
    void Drive(double xSpeed, double ySpeed, double zRotation, double gyroAngle = 0.0f);
    WheelSpeeds DriveIK(double xSpeed, double ySpeed, double zRotation, double gyroAngle = 0.0f);

    protected:

    /*
        Renormalize all wheel speeds if the magnitude of any wheel 
        is greater than 1.0. we cant exceed Maximum Speeds.
    */
    static void Desaturate(estd::span<double> wheelSpeeds);

    double m_deadband = 0.02;
    double m_maxOutput = 1.0;

    bool debug = false;

    private:
    Antrieb* m_frontLeftMotor;
    Antrieb* m_rearLeftMotor;
    Antrieb* m_frontRightMotor;
    Antrieb* m_rearRightMotor;
};