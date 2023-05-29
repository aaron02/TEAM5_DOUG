#pragma once

#include <Arduino.h>

enum LogLevel
{
    LOG_LEVEL_LOG,
    LOG_LEVEL_ERROR
};

class LogHelper
{
public:
    LogHelper(HardwareSerial &serial, unsigned long baudRate);
    bool connect();
    void println(LogLevel level, const String &message);

private:
    HardwareSerial &serial;
    unsigned long baudRate;
};