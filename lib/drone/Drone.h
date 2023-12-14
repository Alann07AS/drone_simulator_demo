// Drone.h
#ifndef DRONE_H
#define DRONE_H

#include <ArduinoJson.h>
#include <vector>

int const DEFAULT_ASSENCION_SPEED = 10;
int const DEFAULT_FLOOR_ALTIDUE = 100;
int const DEFAULT_FLIGHT_ALTIDUE = 200;//250;
int const DEFAULT_MAX_SPEED = 230;//130;

class Drone
{
public:
    static std::vector<Drone> drones;
    enum Status
    {
        LAND,
        STATIC,
        MOVING,
    };

    String name;
    Status status;
    float latitude;
    float longitude;
    float altitude;
    float speed;
    float max_speed;
    float destLatitude;
    float destLongitude;

    typedef void (*FuncPtr)(); // Define a function pointer type

    bool travel();
    bool launch();
    bool land();
    void goToDest(std::function<void()> func);
    void setDest(float destLatitude, float destLongitude);

    // static void newDrone(String droneName, float initLatitude, float initLongitude);
    static Drone *getDroneByName(String droneName);
    Drone(String droneName, float initLatitude, float initLongitude)
    {
        name = droneName;
        latitude = initLatitude;
        longitude = initLongitude;
        altitude = DEFAULT_FLOOR_ALTIDUE;
        max_speed = DEFAULT_MAX_SPEED;
        status = LAND;
        drones.push_back(*this);
    }
};

#endif // DRONE_H
