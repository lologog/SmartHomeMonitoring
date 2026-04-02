#include "secrets.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Topics
const char* mqttTestTopic = "home/office/test";
const char* mqttOfficeLightTopic = "home/office/light";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

BH1750 lightMeter;
Adafruit_BME280 bme(5);

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

    Wire.begin(21, 22);
    lightMeter.begin();

    SPI.begin(18, 19, 23, 5);
    bme.begin();

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

    // Periodic measurements
    float lux = lightMeter.readLightLevel();
    float temperature = bme.readTemperature();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure();
    float altitude = bme.readAltitude(1015.0);
    Serial.print("temperature: ");
    Serial.println(temperature);
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Serial.print("Pressure: ");
    Serial.println(pressure);
    Serial.print("Altitude: ");
    Serial.println(altitude);

    // Send data to the broker
    if (client.connected())
    {
        // "home/office/test"
        Serial.print("Publishing a message on topic: ");
        Serial.println(mqttTestTopic);
        client.publish(mqttTestTopic, "Hello");

        // "home/office/light"
        char lightString[16];
        dtostrf(lux, 1, 2, lightString);
        Serial.print("Publishing a message on topic: ");
        Serial.print(mqttOfficeLightTopic);
        Serial.print(" - Value: ");
        Serial.print(lightString);
        Serial.println(" lx");
        client.publish(mqttOfficeLightTopic, lightString);
    }

    delay(1000);
}