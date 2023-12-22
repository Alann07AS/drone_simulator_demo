#include <WiFi.h>
#include <Drone.h>
#include <Appi.h>

// const char *ssid = "CampusExtended";
// const char *password = "Challenges";

const char *ssid = "Bbox-8CDF86FA";
const char *password = "ZWHAdaqsYxD9FjH3dJ";

// const char *ssid = "Alann_Wifi";
// const char *password = "Alann007";

// const char *ssid = "Livebox-8EF2";
// const char *password = "6D3DEFD9A2156CC62E6A269A52";

void WifiSetup()
{
    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi");
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }
    Serial.print("\nConnected to WiFi. IP: https://");
    Serial.println(WiFi.localIP().toString());
}

// AsyncWebServer server(80);
// AsyncWebServer server(443); // Use port 443 for HTTPS

void setup()
{
    Serial.begin(115200);

    // Set initial drone attributes
    Drone(
        "VTOL",
        49.441329,
        1.072679);
    Drone(
        "MODEL2",
        49.441330,
        1.072679);

    WifiSetup();

    // API DRONE AND APPI INIT
    initAppi();
}

void loop()
{
    // Simulate drone update - You can add your drone update logic here
}
