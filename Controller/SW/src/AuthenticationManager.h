#pragma once

#include <Arduino.h>
#include <string>

#include "Log.h"

class AuthenticationManager
{
public:
    /// @brief Initialize the authentication manager
    static bool initialize();

    /// @brief Try and authenticate the user
    /// @param authentikationKey The key to authenticate the user
    /// @return True if the authentication was successful otherwise false
    static bool authenticateUser(std::string authentikationKey);

    /// @brief Reset the start time
    static void resetStartTime();

    /// @brief Check if the timeout is reached
    /// @param timeout_ms The timeout in ms
    /// @return True if the timeout is reached otherwise false
    static bool isTimeoutReached(unsigned long timeout_ms);

private:
    /// @brief Start time in ms used for the timeout
    static unsigned long startTime_ms;
};