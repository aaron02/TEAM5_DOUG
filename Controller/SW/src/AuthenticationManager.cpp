#include "AuthenticationManager.h"

// Initialize static variables
unsigned long AuthenticationManager::startTime_ms = 0;

bool AuthenticationManager::initialize()
{
    // Write log message
    Log::println(LogType::LOG_TYPE_LOG, "AuthenticationManager", "Initialization of AuthenticationManager successful");

    return true;
}

bool AuthenticationManager::authenticateUser(std::string authentikationKey)
{
    delay(1000);
    return true;
}

void AuthenticationManager::resetStartTime()
{
    startTime_ms = millis();
}

bool AuthenticationManager::isTimeoutReached(unsigned long timeout_ms)
{
    if (millis() - startTime_ms > timeout_ms)
    {
        return true;
    }
    else
    {
        return false;
    }
}