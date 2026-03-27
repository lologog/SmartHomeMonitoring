#include "secrets.h"
#include <WiFi.h>
#include <PubSubClient.h>

// Topics
const char* mqttTestTopic = "home/office/test";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void connectWiFi()
{
    while (WiFi.status() != WL_CONNECTED)
    {
        // Clear previous connection state
        WiFi.disconnect(true, true);
        delay(1000);

        Serial.print("Connecting to WiFi");
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        unsigned long start = millis();
        while(WiFi.status() != WL_CONNECTED && millis() - start < 10000)
        {
            delay(1000);
            Serial.print(".");
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println();
            Serial.println("Connected to WiFi");
            Serial.print("Obtained IP address: ");
            Serial.println(WiFi.localIP());
            return;
        }

        Serial.println();
        Serial.println("WiFi connection failed");
        delay(2000);
        Serial.println("Retrying...");
    }
}

void connectMQTT()
{
    while (!client.connected())
    {
        Serial.print("Connecting with MQTT broker... ");

        // MQTT broker requires client ID - the eFuse MAC should ensure unique ID
        String mqttSubClientID = "mqttSubClientID-";
        mqttSubClientID += String((uint32_t)ESP.getEfuseMac(), HEX);

        if (client.connect(mqttSubClientID.c_str()))
        {
            Serial.println("Connected with the broker");
        }
        else
        {
            Serial.print("Error - rc = ");
            Serial.print(client.state());
            delay(2000);
            Serial.println("Retrying...");
        }
    }
}

void setup() 
{
    Serial.begin(115200);
    delay(5000);

    connectWiFi();

    client.setServer(MQTT_SERVER, MQTT_PORT);
    connectMQTT();
}

void loop() 
{
    if (WiFi.status() != WL_CONNECTED)
    {
        connectWiFi();
    }

    if (!client.connected() && WiFi.status() == WL_CONNECTED)
    {
        connectMQTT();
    }

    // Required for keep-alive mechanism
    client.loop();

    if (client.connected())
    {
        Serial.print("Publishing a message on topic: ");
        Serial.println(mqttTestTopic);
        client.publish(mqttTestTopic, "Hello");
    }

    delay(1000);
}