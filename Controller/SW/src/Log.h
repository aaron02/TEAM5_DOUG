#pragma once

#include "MqttManager.h"

/// @brief The different log types
enum class LogType
{
    LOG_TYPE_LOG,
    LOG_TYPE_ERROR
};

/// @brief This class is used to manage the log
class Log
{
public:
    /// @brief Initialize the log
    /// @param serialBaud Baud rate of the serial connection
    /// @param serialTimeout_ms Timeout for the serial connection in milliseconds
    /// @return True if the initialization was successful otherwise false
    static bool initialize(unsigned long serialBaud, unsigned long serialTimeout_ms);

    /// @brief Print a log message
    /// @param type The type of the log message
    /// @param sender The sender of the message (e.g. the class name)
    /// @param message The message to print
    static void println(LogType type, std:: string sender, std::string message);
};