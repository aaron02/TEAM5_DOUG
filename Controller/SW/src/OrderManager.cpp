#include "OrderManager.h"

// Initialize static variables
std::string OrderManager::deliveryId;
std::vector<Product> OrderManager::products;
std::queue<DeliveryStep> OrderManager::deliveryStepsQueue;

void OrderManager::parse(std::string orderJson)
{
    // Setup json document and deserialize the json string
    DynamicJsonDocument jsonDoc(10000);
    deserializeJson(jsonDoc, orderJson);

    // Shrink the json document to fit the json string to save memory
    jsonDoc.shrinkToFit();

    // Store the delivery id
    deliveryId = jsonDoc["deliveryId"].as<std::string>();

    // Parse the products to pick up
    JsonArray productsToPickUp = jsonDoc["productsToPickUp"];
    int storageLocationRobotCounter = 1;
    for (JsonObject productToPickUp : productsToPickUp)
    {
        Product product;
        product.id = productToPickUp["productId"].as<std::string>();
        product.warehouseLocation = productToPickUp["storageLocation"].as<int>();
        product.state = ProductState::PRODUCT_STATE_NOT_PICKED_UP;

        // Push as many products to pick up into the vector as there are in the order
        for (int i = 0; i < productToPickUp["quantity"].as<int>(); i++)
        {
            // Set the storage location on the robot
            product.storageLocationRobot = storageLocationRobotCounter;

            // Push the product into the vector
            products.push_back(product);

            // Increase the storage location counter
            storageLocationRobotCounter++;
        }
    }

    // Parse the delivery steps
    JsonArray deliverySteps = jsonDoc["deliverySteps"];
    for (JsonObject deliveryStep : deliverySteps)
    {
        DeliveryStep step;
        step.id = deliveryStep["id"].as<int>();

        step.state = DeliveryStepState::DELIVERY_STEP_STATE_NOT_STARTED;

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
        step.productIdToPlace = deliveryStep["productId"].as<std::string>();
        step.plannedDeliveryTime = deliveryStep["plannedDeliveryTime"].as<std::string>();

        Coordinates coordinates;
        coordinates.x = deliveryStep["coordinates"]["x"].as<int>();
        coordinates.y = deliveryStep["coordinates"]["x"].as<int>();

        step.coordinates = coordinates;

        deliveryStepsQueue.push(step);
    }
}

bool OrderManager::hasProductToPickUp()
{
    // Check if there are products left to pick up
    for (Product product : products)
    {
        if (product.state == ProductState::PRODUCT_STATE_NOT_PICKED_UP)
        {
            return true;
        }
    }

    return false;
}

Product &OrderManager::getNextProductToPickUp()
{
    // Get the next product to pick up
    for (Product &product : products)
    {
        if (product.state == ProductState::PRODUCT_STATE_NOT_PICKED_UP)
        {
            return product;
        }
    }

    // If there are no products left to pick up, return the first product
    return products[0];
}

bool OrderManager::hasDeliveryStepLeft()
{
    return !deliveryStepsQueue.empty();
}

DeliveryStep &OrderManager::getCurrentDeliveryStep()
{
    // Check if the first delivery step is finished
    if (deliveryStepsQueue.front().state == DeliveryStepState::DELIVERY_STEP_STATE_FINISHED)
    {
        // Remove the first delivery step
        deliveryStepsQueue.pop();
    }

    // Get the current delivery step
    return deliveryStepsQueue.front();
}

std::string OrderManager::getdeliveryId()
{
    return deliveryId;
}

int OrderManager::getStorageLocationRobotByProductId(std::string productId)
{
    // Get the storage location on the robot by the product id
    for (Product product : products)
    {
        if (product.id == productId)
        {
            return product.storageLocationRobot;
        }
    }

    return -1;
}