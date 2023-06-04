#pragma once

#include "LogHelper.h"

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

class Waypoint
{
public:
 Waypoint();
    Waypoint(WaypointType waypointType, Coordinates coordinates, int id);
    Waypoint(WaypointType waypointType, Coordinates coordinates, int id, String productId);
    Waypoint(WaypointType waypointType, Coordinates coordinates, int id, String productId, String authorizationKey);

    WaypointType GetWaypointType();
    Coordinates GetCoordinates();

private:
    WaypointType waypointType;
    Coordinates coordinates;
    int id;
    String productId;
    String authorizationKey;
};