// Drone.cpp
#include <Drone.h>

std::vector<Drone> Drone::drones; // Define the static member

// String generateDroneInfo()
// {
//     DynamicJsonDocument doc(200);
//     doc["name"] = drone.name;
//     doc["status"] = drone.status;
//     doc["latitude"] = drone.latitude;
//     doc["longitude"] = drone.longitude;
//     doc["speed"] = drone.speed;
//     doc["destLatitude"] = drone.destLatitude;
//     doc["destLongitude"] = drone.destLongitude;

//     String jsonString;
//     serializeJson(doc, jsonString);

//     return jsonString;
// }

// Méthode pour calculer la distance en mètres entre deux points géographiques
float calculateHaversineDistance(float lat1, float lon1, float lat2, float lon2)
{
    const float R = 6371000.0f; // Rayon moyen de la Terre en mètres
    float dLat = (lat2 - lat1) * M_PI / 180.0f;
    float dLon = (lon2 - lon1) * M_PI / 180.0f;
    float a = std::sin(dLat / 2.0f) * std::sin(dLat / 2.0f) +
              std::cos(lat1 * M_PI / 180.0f) * std::cos(lat2 * M_PI / 180.0f) *
                  std::sin(dLon / 2.0f) * std::sin(dLon / 2.0f);
    float c = 2.0f * std::atan2(std::sqrt(a), std::sqrt(1.0f - a));
    return R * c;
}
float calculateAzimuth(float lat1, float lon1, float lat2, float lon2)
{
    // Conversion des coordonnées de degrés à radians
    lat1 = lat1 * M_PI / 180.0;
    lon1 = lon1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;
    lon2 = lon2 * M_PI / 180.0;

    // Calcul de la différence de longitude
    float dlon = lon2 - lon1;

    // Calcul de l'azimut
    float azimuth = atan2(
        sin(dlon) * cos(lat2),
        cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dlon));

    // // Conversion de l'azimut en degrés
    // float azimuthDegrees = azimuth * 180.0 / M_PI;

    // // Assurez-vous que l'azimut est dans la plage [0, 360)
    // azimuthDegrees = fmod((azimuthDegrees + 360.0), 360.0);

    return azimuth;
}

bool Drone::travel()
{
    if (status != STATIC)
    {
        return false;
    }

    status = MOVING;
    float speedms = max_speed * 1000.0 / 3600.0; // convertion m/S
    // speed = speedms;
    float azimut = calculateAzimuth(latitude, longitude, destLatitude, destLongitude);
    speed = azimut;
    // implementer aceleration 2m/s ?
    // while (std::abs(latitude - destLatitude) > 0.0001 || std::abs(longitude - destLongitude) > 0.0001)
    while (std::abs(destLongitude - longitude) > 0.001 || std::abs(destLatitude - latitude) > 0.001)
    {
        // Rayon moyen de la Terre en mètres
        const float earthRadius = 6371000.0;

        // Conversion de la latitude et de la longitude de degrés à radians
        float latRad = latitude * M_PI / 180.0;
        float lonRad = longitude * M_PI / 180.0;

        // Calcul des nouvelles coordonnées
        latitude = latitude + (speedms * cos(azimut) * 50) / earthRadius;
        longitude = longitude + (speedms * sin(azimut) * 50) / (earthRadius * cos(latRad));

        // Simulate the passage of time
        // sleep(1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    longitude = destLongitude;
    latitude = destLatitude;
    status = STATIC; // Set status back to STATIC when the travel is complete
    return true;
}

bool Drone::launch()
{ // lauch verticaly drone to flight height while drone is not up enouth
    if (status != LAND)
    {
        return false;
        // Serial.println("ETAPE 5 C   NOT LAND LAUCH ?????");
    }
    // Serial.println("START MOVING UP");
    status = MOVING;
    // Serial.println("ETAPE 5 C   LAUCH ?????");
    while (altitude < DEFAULT_FLIGHT_ALTIDUE)
    {
        // sleep(1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        altitude += DEFAULT_ASSENCION_SPEED;
        // Serial.println("MOVING UP");
    }
    // Serial.println("ETAPE 5 C   LAUCH OK");
    // Serial.println("STOP MOVING UP");
    status = STATIC;
    return true;
};

bool Drone::land()
{ // land drone with max speed while height secure area (20m) and low speed while dont touch the floor
    if (status != STATIC)
    {
        return false;
    }
    status = MOVING;
    while (altitude > DEFAULT_FLOOR_ALTIDUE + 20)
    {
        // sleep(1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        altitude -= DEFAULT_ASSENCION_SPEED;
    }
    // the dist between floor and drone sub by 5
    float const SPEED = (altitude - static_cast<float>(DEFAULT_ASSENCION_SPEED)) / 5.;
    while (altitude > DEFAULT_FLOOR_ALTIDUE)
    {
        // sleep(1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        altitude -= SPEED;
    }
    altitude = DEFAULT_FLOOR_ALTIDUE;
    status = LAND;
    return true;
};

void Drone::setDest(float newDestLatitude, float newDestLongitude)
{
    destLatitude = newDestLatitude;
    destLongitude = newDestLongitude;
};

Drone *Drone::getDroneByName(String droneName)
{
    for (Drone &drone : Drone::drones)
    {
        if (drone.name == droneName)
        {
            return &drone;
        }
    }
    return NULL;
};

void Drone::goToDest(std::function<void()> func)
{
    // Capture necessary variables by value
    auto capturedFunc = [this, func]()
    {
        this->launch();
        this->travel();
        this->land();
        func();
        vTaskDelete(NULL);
    };

    // Use the lambda function pointer as a parameter for xTaskCreate
    xTaskCreate(
        [](void *param)
        {
            auto *lambdaFunc = static_cast<std::function<void()> *>(param);
            (*lambdaFunc)();   // Invoke the lambda function
            delete lambdaFunc; // Release the allocated memory
        },
        "goToDest",
        800,                                     // Stack size (adjust as needed)
        new std::function<void()>(capturedFunc), // Allocate memory for lambda
        1,
        NULL);
}
