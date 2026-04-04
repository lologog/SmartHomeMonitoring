#include "secrets.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SPI.h>
#include <BH1750.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Topics
const char* mqttStatusTopic = nullptr;
const char* mqttMeasurementsTopic = nullptr;

const char* mqttOfficeStatusTopic = "home/office/status";
const char* mqttLivingRoomStatusTopic = "home/livingroom/status";
const char* mqttKitchenStatusTopic = "home/kitchen/status";
const char* mqttBathroomStatusTopic = "home/bathroom/status";

const char* mqttOfficeMeasurementsTopic = "home/office/measurements";
const char* mqttLivingRoomMeasurementsTopic = "home/livingroom/measurements";
const char* mqttKitchenMeasurementsTopic = "home/kitchen/measurements";
const char* mqttBathroomMeasurementsTopic = "home/bathroom/measurements";

// Pin definitions
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

#define SPI_SCK_PIN 18
#define SPI_MISO_PIN 19
#define SPI_MOSI_PIN 23
#define BME_CS_PIN 5

// Sensor / payload config
#define SEA_LEVEL_PRESSURE_HPA 1015.0
#define JSON_PAYLOAD_SIZE 256

WiFiClient wifiClient;
PubSubClient client(wifiClient);

BH1750 lightMeter;
Adafruit_BME280 bme(BME_CS_PIN);

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
        while (WiFi.status() != WL_CONNECTED && millis() - start < 10000)
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
    // Prevent using uninitialized MQTT topic pointers
    if (mqttStatusTopic == nullptr || mqttMeasurementsTopic == nullptr)
    {
        Serial.println("Error - MQTT topics not initialized");
        return;
    }

    while (!client.connected())
    {
        Serial.println("Connecting with MQTT broker... ");

        // MQTT broker requires client ID - the eFuse MAC should ensure unique ID
        String mqttSubClientID = "mqttSubClientID-";
        mqttSubClientID += String((uint32_t)ESP.getEfuseMac(), HEX);

        if (client.connect(mqttSubClientID.c_str(), mqttStatusTopic, 0, true, "offline"))
        {
            Serial.println("Connected with the broker");
            client.publish(mqttStatusTopic, "online", true);
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
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
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
    SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, BME_CS_PIN);
    if (!bme.begin())
    {
        Serial.println("ERROR: BME280 not detected");
        while (1)
        {
            delay(100);
        }
    }
    Serial.println("BME280 detected");

    WiFi.mode(WIFI_STA);
    connectWiFi();

    mqttStatusTopic = mqttKitchenStatusTopic;
    mqttMeasurementsTopic = mqttKitchenMeasurementsTopic;
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
    float altitude = bme.readAltitude(SEA_LEVEL_PRESSURE_HPA);

    // Send data to the broker
    if (client.connected())
    {
        char payload[JSON_PAYLOAD_SIZE];
        snprintf(payload, sizeof(payload),
                "{\n"
                "  \"light\": %.2f,\n"
                "  \"temperature\": %.2f,\n"
                "  \"humidity\": %.2f,\n"
                "  \"pressure\": %.2f,\n"
                "  \"altitude\": %.2f\n"
                "}",
                lux, temperature, humidity, pressure, altitude);

        if (client.publish(mqttMeasurementsTopic, payload))
        {
            Serial.print("Publishing on topic: ");
            Serial.println(mqttMeasurementsTopic);
            Serial.println(payload);
        }
        else
        {
            Serial.println("Publish failed");
        }
    }

    delay(1000);
}