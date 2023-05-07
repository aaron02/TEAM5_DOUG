/*
Copyright (c) 2023-2023 AÜP TEAM 5 HIGH5DYNAMICS
*/

#pragma once

#include "Defnies.h"

enum Severity
{
    NONE,
    INFO,
    WARNING,
    FAILURE,
    FATAL
};

enum MessageType
{
    TRACE,
    DEBUG,
    MINOR,
    MAJOR
};

class Logger
{
    MessageType minimumMessageType = MessageType::DEBUG;

public:
    Logger(Logger&&) = delete;
    Logger(Logger const&) = delete;
    Logger& operator=(Logger&&) = delete;
    Logger& operator=(Logger const&) = delete;

    static Logger& getInstance();

    void setLogType(MessageType type) { minimumMessageType = type; }

    void trace(const char* message, ...);

    void debug(const char* message, ...);

    void info(const char* message, ...);

    void warning(const char* message, ...);

    void failure(const char* message, ...);

    void fatal(const char* message, ...);

    void log(Severity severity, MessageType messageType, const char* message, ...);

    void log(Severity severity, MessageType messageType, const char* message, va_list arguments);

    void createLogMessage(char* result, Severity severity, MessageType messageType, const char* message, va_list arguments);

    std::string getMessageTypeText(MessageType messageType);
    std::string getSeverityText(Severity severity);

    std::string getCurrentDateTimeString();
    std::string getCurrentTimeString();

private:
    Logger() = default;
    ~Logger() = default;
};

#define sLogger Logger::getInstance()
