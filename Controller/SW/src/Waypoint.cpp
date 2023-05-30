#include "Waypoint.h"

Waypoint::Waypoint()
{
}

Waypoint::Waypoint(WaypointType waypointType, Coordinates coordinates, int id) : waypointType(waypointType), coordinates(coordinates), id(id)
{
}

Waypoint::Waypoint(WaypointType waypointType, Coordinates coordinates, int id, String productId) : waypointType(waypointType), coordinates(coordinates), id(id), productId(productId)
{
}

Waypoint::Waypoint(WaypointType waypointType, Coordinates coordinates, int id, String productId, String authorizationKey) : waypointType(waypointType), coordinates(coordinates), id(id), productId(productId), authorizationKey(authorizationKey)
{
}

WaypointType Waypoint::GetWaypointType()
{
    return waypointType;
}

Coordinates Waypoint::GetCoordinates()
{
    return coordinates;
}