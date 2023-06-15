#include "Log.h"

bool Log::initialize(unsigned long serialBaud, unsigned long serialTimeout_ms)
{
    // Initialize the serial connection
    Serial.begin(serialBaud);

    // Wait for the serial connection to be established with a timeout of 2 seconds
    unsigned long startTime = millis();
    while (!Serial && millis() - startTime < serialTimeout_ms)
    {
    }

    // Check if the serial connection is established
    if (!Serial)
    {
        // Return false if the serial connection is not established
        return false;
    }

    // Write first log message
    println(LogType::LOG_TYPE_LOG, "Log", "Serial connection established");

    return true;
}

void Log::println(LogType type, std::string sender, std::string message)
{
    // Generate the log message
    std::string prefix = (type == LogType::LOG_TYPE_LOG) ? "[LOG] " : "[ERROR] ";
    std::string fullMessage = "[" + std::to_string(millis()) + "] " + prefix + "[" + sender + "] " + message;

    // Print the log message
    Serial.println(fullMessage.c_str());
}