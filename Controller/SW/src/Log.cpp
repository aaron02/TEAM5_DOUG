#include "Log.h"


void Log::initialize(unsigned long serialBaud)
{
    Serial.begin(serialBaud);
}

void Log::println(LogType type, String message)
{
    Serial.println(((type == LogType::LOG_TYPE_LOG) ? "[log] " : "[error] ") + message);
}