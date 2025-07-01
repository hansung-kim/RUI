#include <cmath>
#include <DeadReckoning.h>

DeadReckoning::DeadReckoning() {

}

DeadReckoning::~DeadReckoning() {

}

NextPosition_t DeadReckoning::PredictNextPosition(
            double latitude,         // degrees
            double longitude,        // degrees
            double altitude,         // in feet
            double speed,            // in meters per second
            double heading,          // in degrees (0 = North, clockwise)
            double verticalRate,     // in feet per minute
            double deltaTimeSec      // in seconds since last known position
)
{
    const double earthRadius = 6371000.0; // in meters

    double distance = speed * deltaTimeSec;

    // Convert heading to radians
    double headingRad = heading * M_PI / 180.0;

    // Compute displacement
    double dx = distance * sin(headingRad); // 경도 방향
    double dy = distance * cos(headingRad); // 위도 방향

    // Convert back to lat/lon
    double new_latitude = latitude + (dy / earthRadius) * (180.0 / M_PI);
    double new_longitude = longitude + (dx / (earthRadius * cos(latitude * M_PI / 180.0))) * (180.0 / M_PI);

    // Altitude calculation
    double new_altitude = altitude + verticalRate * (deltaTimeSec / 60.0); // ft/min × min

    NextPosition_t result = { new_latitude, new_longitude, new_altitude };
    return result;
}
