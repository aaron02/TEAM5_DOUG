#pragma once

#include <queue>
#include <vector>
#include <string>
#include <ArduinoJson.h>

#include "Log.h"
#include "Coordinates.h"
#include "Product.h"

enum WaypointType
{
    WAYPOINT_PARK_POSITION,
    WAYPOINT_PARK_DISTRIBUTION_CENTER,
    WAYPOINT,
    WAYPOINT_HANDOVER,
    WAYPOINT_DEPOSIT
};

enum DeliveryStepState
{
    DELIVERY_STEP_STATE_NOT_STARTED,
    DELIVERY_STEP_STATE_DRIVING,
    DELIVERY_STEP_STATE_PICKING,
    DELIVERY_STEP_STATE_AUTHENTICATING,
    DELIVERY_STEP_STATE_PLACING,
    DELIVERY_STEP_STATE_FINISHED
};

struct DeliveryStep
{
    int id;
    DeliveryStepState state;
    WaypointType waypointType;
    std::string authorizationKey;
    std::string productIdToPlace;
    std::string plannedDeliveryTime;
    Coordinates coordinates;
};

class OrderManager
{
public:
    /// @brief Parse the order from a json string
    static void parse(std::string orderJson);

    /// @brief  Check if there are products left to pick up
    static bool hasProductToPickUp();

    /// @brief  Get the next product to pick up as reference
    static Product &getNextProductToPickUp();

    /// @brief check if there are delivery steps left
    static bool hasDeliveryStepLeft();

    ///  @brief Get a the current delivery step as reference
    static DeliveryStep &getCurrentDeliveryStep();

    /// @brief Get the id of the order
    static std::string getdeliveryId();

    /// @brief Get the first storage location of a product on the robot by product id
    static int getStorageLocationRobotByProductId(std::string productId);

private:
    /// @brief  The id of the order
    static std::string deliveryId;

    /// @brief  Vector with all pruducts in the order
    static std::vector<Product> products;

    /// @brief  Queue with all delivery steps in the order
    static std::queue<DeliveryStep> deliveryStepsQueue;
};