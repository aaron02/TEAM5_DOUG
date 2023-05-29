#include "LogHelper.h"

LogHelper::LogHelper(HardwareSerial &serial, unsigned long baudRate) : serial(serial), baudRate(baudRate)
{
}

bool LogHelper::connect()
{
    // Init port
    serial.begin(baudRate);

    // Check if port is ready
    if (!serial)
    {
        return false;
    }

    LogHelper::println(LOG_LEVEL_LOG, "Log successfully started");

    return true;
}

void LogHelper::println(LogLevel level, const String &message)
{
    switch (level)
    {
    case LOG_LEVEL_LOG:
        serial.print("[LOG] ");
        break;
    case LOG_LEVEL_ERROR:
        serial.print("[ERROR] ");
        break;
    }
    serial.println(message);
}