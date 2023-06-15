#pragma once

#include <string>

struct MqttMessage
{
    std::string topic;
    std::string payload;
};