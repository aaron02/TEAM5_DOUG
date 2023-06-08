#include "Order.h"

Order::Order()
{
}

void Order::parse(std::string orderJson)
{
    DynamicJsonDocument doc(10000);
    deserializeJson(doc, orderJson);
    doc.shrinkToFit();

    std::string stringTotPrint;
    serializeJson(doc, stringTotPrint);

    Log::println(LogType::LOG_TYPE_LOG, stringTotPrint);

    deliveryId = doc["deliveryId"].as<std::string>();

    JsonArray productsToPickUp = doc["productsToPickUp"];
    for (JsonObject productToPickUp : productsToPickUp)
    {
        Product product;
        product.storageLocation = productToPickUp["storageLocation"].as<int>();
        product.quantity = productToPickUp["quantity"].as<int>();
        productsToPickUpMap[productToPickUp["productId"].as<std::string>()] = product;
    }

    JsonArray deliverySteps = doc["deliverySteps"];
    for (JsonObject deliveryStep : deliverySteps)
    {
        DeliveryStep step;
        step.id = deliveryStep["id"].as<int>();

        if (deliveryStep["type"].as<std::string>() == "waypoint")
        {
            step.waypointType = WaypointType::WAYPOINT;
        }
        else if (deliveryStep["type"] == "deposit")
        {
            step.waypointType = WaypointType::WAYPOINT_DEPOSIT;
        }
        else if (deliveryStep["type"] == "handOver")
        {
            step.waypointType = WaypointType::WAYPOINT_HANDOVER;
        }
        else if (deliveryStep["type"] == "distributionCenter")
        {
            step.waypointType = WaypointType::WAYPOINT_PARK_DISTRIBUTION_CENTER;
        }
        else if (deliveryStep["type"] == "parkPosition")
        {
            step.waypointType = WaypointType::WAYPOINT_PARK_POSITION;
        }

        step.authorizationKey = deliveryStep["authorizationKey"].as<std::string>();
        step.productId = deliveryStep["productId"].as<std::string>();
        step.plannedDeliveryTime = deliveryStep["plannedDeliveryTime"].as<std::string>();

        Coordinates coordinates;
        coordinates.x = deliveryStep["coordinates"]["x"].as<int>();
        coordinates.y = deliveryStep["coordinates"]["x"].as<int>();

        step.coordinates = coordinates;

        deliveryStepsQueue.push(step);
    }
}

DeliveryStep Order::getNextDeliveryStep()
{
    DeliveryStep deliveryStep = deliveryStepsQueue.front();
    deliveryStepsQueue.pop();
    return deliveryStep;
}

bool Order::hasDeliveryStep()
{
    return !deliveryStepsQueue.empty();
}

std::string Order::getdeliveryId()
{
    return deliveryId;
}

Product Order::getProduct(std::string productId)
{
    return productsToPickUpMap[productId];
}