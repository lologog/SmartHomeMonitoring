#include "secrets.h"
#include <WiFi.h>

void connectWiFi()
{
    Serial.print("Connecting with WiFi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Connected to WiFi");
    Serial.print("Obtained IP address: ");
    Serial.println(WiFi.localIP());
}

void setup() 
{
    Serial.begin(115200);
    delay(1000);

    connectWiFi();
}

void loop() 
{
    delay(1000);
}