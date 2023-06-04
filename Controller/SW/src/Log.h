#pragma once

#include "MqttManager.h"

/**
 * @brief Enum representing the type of log message.
 */
enum class LogType
{
    LOG_TYPE_LOG,  /**< Log message type. */
    LOG_TYPE_ERROR /**< Error message type. */
};

/**
 * @brief Class for logging messages.
 */
class Log
{
public:
    /**
     * @brief Initializes the Log class.
     * @param serialBaud The baud rate for serial communication.
     */
    static void initialize(unsigned long serialBaud);

    /**
     * @brief Prints a log message.
     * @param type The type of the log message.
     * @param message The log message.
     */
    static void println(LogType type, String message);
};