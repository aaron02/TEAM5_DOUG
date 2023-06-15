#pragma once

#include <string>

// The different states of the product
enum ProductState
{
    PRODUCT_STATE_NOT_PICKED_UP,
    PRODUCT_STATE_PICKED_UP,
    PRODUCT_STATE_PLACED
};

/// @brief This struct represents a product
struct Product
{
    /// @brief The id of the product
    std::string id;

    /// @brief  The warehouse location of the product (1-4)
    int warehouseLocation;

    /// @brief The Storage location of the product on the robot (1-4) 
    int storageLocationRobot;

    /// @brief The state of the product
    ProductState state;
};