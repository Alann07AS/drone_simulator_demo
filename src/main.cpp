#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWebServerSecure.h>  // Include the secure server library
#include <Drone.h>
#include <Appi.h>

// const char *ssid = "CampusExtended";
const char *ssid = "Alann_Wifi";
// const char *password = "Challenges";
const char *password = "Alann007";

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
  Serial.print("\nConnected to WiFi. IP: http://");
  Serial.println(WiFi.localIP().toString());
}

// AsyncWebServer server(80);
AsyncWebServer server(443);  // Use port 443 for HTTPS

void setup()
{
  Serial.begin(115200);

  // Set initial drone attributes
  Drone(
      "VTOL",
      44.741436, 
      4.745028
  );
  WifiSetup();

  // API DRONE AND APPI INIT
  initAppi(&server);

}

void loop()
{
  // Simulate drone update - You can add your drone update logic here
}
