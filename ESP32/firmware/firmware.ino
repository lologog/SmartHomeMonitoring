#include "secrets.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SPI.h>
#include <BH1750.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Topics
const char* mqttTestTopic = "home/office/test";
const char* mqttOfficeMeasurementsTopic = "home/office/measurements";

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

    // BH1750 - I2C
    Wire.begin(21, 22);
    if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
    {
        Serial.println("ERROR: BH1750 not detected");
        while (1)
        {
            delay(100);
        }
    }
    Serial.println("BH1750 detected");

    // BME280 - SPI
    SPI.begin(18, 19, 23, 5);
    if (!bme.begin())
    {
        Serial.println("ERROR: BME280 not detected");
        while (1)
        {
            delay(100);
        }
    }
    Serial.println("BME280 detected");

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
    float pressure = bme.readPressure() / 100.0F;
    float altitude = bme.readAltitude(1015.0);

    // Send data to the broker
    if (client.connected())
    {
        // "home/office/measurements"
        char payload[256];
        snprintf(payload, sizeof(payload),
                "{\n"
                "  \"light\": %.2f,\n"
                "  \"temperature\": %.2f,\n"
                "  \"humidity\": %.2f,\n"
                "  \"pressure\": %.2f,\n"
                "  \"altitude\": %.2f\n"
                "}",
                lux, temperature, humidity, pressure, altitude);
        if (client.publish(mqttOfficeMeasurementsTopic, payload))
        {
            Serial.print("Publishing on topic: ");
            Serial.println(mqttOfficeMeasurementsTopic);
            Serial.println(payload);
        }
        else
        {
            Serial.println("Publish failed");
        }
    }

    delay(1000);
}