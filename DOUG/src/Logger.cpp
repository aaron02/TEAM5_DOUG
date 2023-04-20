/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#include "Logger.h"

Logger& Logger::getInstance()
{
    static Logger mInstance;
    return mInstance;
}

void Logger::trace(const char* message, ...)
{
    va_list arguments;
    va_start(arguments, message);
    log(Severity::INFO, MessageType::TRACE, message, arguments);
    va_end(arguments);
}

void Logger::debug(const char* message, ...)
{
    va_list arguments;
    va_start(arguments, message);
    log(Severity::INFO, MessageType::DEBUG, message, arguments);
    va_end(arguments);
}

void Logger::info(const char* message, ...)
{
    va_list arguments;
    va_start(arguments, message);
    log(Severity::INFO, MessageType::MINOR, message, arguments);
    va_end(arguments);
}

void Logger::warning(const char* message, ...)
{
    va_list arguments;
    va_start(arguments, message);
    log(Severity::WARNING, MessageType::MINOR, message, arguments);
    va_end(arguments);
}

void Logger::failure(const char* message, ...)
{
    va_list arguments;
    va_start(arguments, message);
    log(Severity::FAILURE, MessageType::MAJOR, message, arguments);
    va_end(arguments);
}

void Logger::fatal(const char* message, ...)
{
    va_list arguments;
    va_start(arguments, message);
    log(Severity::FATAL, MessageType::MAJOR, message, arguments);
    va_end(arguments);
}

void Logger::log(Severity severity, MessageType messageType, const char* message, ...)
{
    va_list arguments;
    va_start(arguments, message);
    log(severity, messageType, message, arguments);
    va_end(arguments);
}

void Logger::log(Severity severity, MessageType messageType, const char* message, va_list arguments)
{
    if (this->minimumMessageType > messageType)
        return;

    char logMessage[254];
    createLogMessage(logMessage, severity, messageType, message, arguments);

    //std::cout << logMessage << "\r\n";    // not working on Teensy
    Serial.println(logMessage);
}

void Logger::createLogMessage(char* result, Severity severity, MessageType messageType, const char* message, va_list arguments)
{
    char formattedMessage[254];
    vsnprintf(formattedMessage, 254, message, arguments);

    std::string currentTime = getCurrentTimeString();
    std::string severityText = getSeverityText(severity);
    std::string messageTypeText = getMessageTypeText(messageType);

    sprintf(result, "%s %s%s: %s", currentTime.c_str(), severityText.c_str(), messageTypeText.c_str(), formattedMessage);
}

std::string Logger::getMessageTypeText(MessageType messageType)
{
    switch (messageType)
    {
    case TRACE:
        return "[TRACE]";
    case DEBUG:
        return "[DEBUG]";
    case MAJOR:
        return "[MAJOR]";
    default:
        return "";
    }
}

std::string Logger::getSeverityText(Severity severity)
{
    switch (severity)
    {
    case WARNING:
        return "[WARNING]";
    case FAILURE:
        return "[ERROR]";
    case FATAL:
        return "[FATAL]";
    case INFO:
    default:
        return "[INFO]";
    }
}

std::string Logger::getCurrentDateTimeString()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    char buff[20];
    char string = strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&in_time_t));
    std::string str(buff);
    return str;
}

std::string Logger::getCurrentTimeString()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    char buff[20];
    char string = strftime(buff, 20, "%H:%M:%S", localtime(&in_time_t));
    std::string str(buff);
    return str;
}
    