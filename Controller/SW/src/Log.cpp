#include "Log.h"


void Log::initialize(unsigned long serialBaud)
{
    Serial.begin(serialBaud);
}

void Log::println(LogType type, std::string message)
{
    std::string prefix = (type == LogType::LOG_TYPE_LOG) ? "[log] " : "[error] ";
    std::string fullMessage = prefix + message;
    Serial.println(fullMessage.c_str());
}