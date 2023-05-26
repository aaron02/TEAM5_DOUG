/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "DriveTrain.h"
#undef abs

DriveTrain::DriveTrain(Antrieb &frontLeftMotor, Antrieb &rearLeftMotor, Antrieb &frontRightMotor, Antrieb &rearRightMotor) : m_frontLeftMotor(&frontLeftMotor),
                                                                                                                             m_rearLeftMotor(&rearLeftMotor),
                                                                                                                             m_frontRightMotor(&frontRightMotor),
                                                                                                                             m_rearRightMotor(&rearRightMotor)
{
}

DriveTrain::DriveTrain(Antrieb2& mecanumDrive) : m_mecanumDrive(&mecanumDrive)
{

}

DriveTrain::~DriveTrain()
{
}

double DriveTrain::ApplyDeadband(double value, double deadband, double maxMagnitude = double(1.0))
{
  double magnitude = std::abs(value);

  if (magnitude > deadband)
  {
    if (maxMagnitude / deadband > 1.0E12)
      return value > double(0.0) ? value - deadband : value + deadband;
    if (value > double(0.0))
      return maxMagnitude * (value - deadband) / (maxMagnitude - deadband);
    else
      return maxMagnitude * (value + deadband) / (maxMagnitude - deadband);
  }
  else
    return double(0.0);
}

void DriveTrain::SetDeadband(double deadband)
{
  m_deadband = deadband;
}
void DriveTrain::SetMaxOutput(double maxOutput)
{
  m_maxOutput = maxOutput;
}

void DriveTrain::Desaturate(estd::span<double> wheelSpeeds)
{
  double maxMagnitude = std::abs(wheelSpeeds[0]);

  for (size_t i = 1; i < wheelSpeeds.size(); i++)
  {
    double temp = std::abs(wheelSpeeds[i]);
    if (maxMagnitude < temp)
    {
      maxMagnitude = temp;
    }
  }

  if (maxMagnitude > 1.0)
  {
    for (size_t i = 0; i < wheelSpeeds.size(); i++)
    {
      wheelSpeeds[i] = wheelSpeeds[i] / maxMagnitude;
    }
  }
}

void DriveTrain::Drive(double xSpeed, double ySpeed, double zRotation, double gyroAngle)
{
  xSpeed = ApplyDeadband(xSpeed, m_deadband);
  ySpeed = ApplyDeadband(ySpeed, m_deadband);

  WheelSpeeds speeds = DriveIK(xSpeed, ySpeed, zRotation, gyroAngle);
  
/*
  m_frontLeftMotor->setSpeed(speeds.frontLeft * m_maxOutput);
  m_frontRightMotor->setSpeed(speeds.frontRight * m_maxOutput);
  m_rearLeftMotor->setSpeed(speeds.rearLeft * m_maxOutput);
  m_rearRightMotor->setSpeed(speeds.rearRight * m_maxOutput);
*/

  // New Drive Functions
  m_mecanumDrive->setSpeed(MotorType::kFrontLeft, speeds.frontLeft * m_maxOutput);
  m_mecanumDrive->setSpeed(MotorType::kFrontRight, speeds.frontRight * m_maxOutput);
  m_mecanumDrive->setSpeed(MotorType::kRearLeft, speeds.rearLeft * m_maxOutput);
  m_mecanumDrive->setSpeed(MotorType::kRearRight, speeds.rearRight * m_maxOutput);
}

WheelSpeeds DriveTrain::DriveIK(double xSpeed, double ySpeed, double zRotation, double gyroAngle)
{
  xSpeed = constrain(xSpeed, -1.0, 1.0);
  ySpeed = constrain(ySpeed, -1.0, 1.0);

  double wheelSpeeds[4];

  // Compensate for gyro angle.
  /* Todo Gyro Compensation when needed */

  wheelSpeeds[kFrontLeft] = xSpeed + ySpeed + zRotation;
  wheelSpeeds[kFrontRight] = xSpeed - ySpeed - zRotation;
  wheelSpeeds[kRearLeft] = xSpeed - ySpeed + zRotation;
  wheelSpeeds[kRearRight] = xSpeed + ySpeed - zRotation;

  Desaturate(wheelSpeeds);

  WheelSpeeds speeds;

  speeds.frontLeft = wheelSpeeds[kFrontLeft];
  speeds.frontRight = wheelSpeeds[kFrontRight];
  speeds.rearLeft = wheelSpeeds[kRearLeft];
  speeds.rearRight = wheelSpeeds[kRearRight];

  return speeds;
}