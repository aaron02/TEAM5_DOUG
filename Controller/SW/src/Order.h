#pragma once

#include <queue>
#include <unordered_map>
#include <string>
#include <ArduinoJson.h>

#include "Log.h"

struct Product
{
    int storageLocation;
    int quantity;
};

enum WaypointType
{
    WAYPOINT_PARK_POSITION,
    WAYPOINT_PARK_DISTRIBUTION_CENTER,
    WAYPOINT,
    WAYPOINT_HANDOVER,
    WAYPOINT_DEPOSIT
};

struct Coordinates
{
    int x;
    int y;
};

struct DeliveryStep
{
    int id;
    WaypointType waypointType;
    std::string authorizationKey;
    std::string productId;
    std::string plannedDeliveryTime;
    Coordinates coordinates;
};

class Order
{
public:
    Order();
    void parse(std::string orderJson);
    DeliveryStep getNextDeliveryStep();
    bool hasDeliveryStep();
    std::string getdeliveryId();
    Product getProduct(std::string productId);

private:
    std::string deliveryId;
    std::unordered_map<std::string, Product> productsToPickUpMap;
    std::queue<DeliveryStep> deliveryStepsQueue;
};