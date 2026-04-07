# ESP32

Firmware for the ESP32 responsible for collecting environmental data and sending it to the MQTT broker.

## Overview

This module handles communication between the sensors and the backend system.

It:
- connects to Wi-Fi,
- connects to the MQTT broker,
- reads data from sensors:
  - BH1750 (light),
  - BME280 (temperature, humidity, pressure, altitude),
- publishes device status,
- sends measurements as JSON.

## Prototype

Below is a prototype setup of the ESP32 with connected sensors.

ADD PHOTO LATER

## Required libraries
- WiFi
- PubSubClient
- Wire
- SPI
- BH1750
- Adafruit Unified Sensor
- Adafruit BME280

## Configuration

Before uploading the firmware, create a `secrets.h` file inside the `firmware/` folder.

### Example

```c
#pragma once

#define WIFI_SSID "your_wifi_name"
#define WIFI_PASSWORD "your_wifi_password"

#define MQTT_SERVER "192.168.1.25"
#define MQTT_PORT 1883
```

## Hardware connections

### BH1750 (I2C)
- SDA -> GPIO 21
- SCL -> GPIO 22

### BME280 (SPI)
- SCK -> GPIO 18
- MISO -> GPIO 19
- MOSI -> GPIO 23
- CS -> GPIO 5

## MQTT topics

### Status

- home/+/status

### Measurements

- home/+/measurements

## Payload format

Example message:

```json
{
  "light": 785.83,
  "temperature": 24.84,
  "humidity": 32.53,
  "pressure": 999.72,
  "altitude": 127.74
}
```

## Uploading firmware

1. Open `firmware.ino` in Arduino IDE.
2. Install the required libraries.
3. Create and configure `secrets.h`.
4. Select the ESP32 board and port.
5. Choose the MQTT topics in the code.
6. Upload the code.

## Notes

- Device publishes:
  - `online` after connecting to MQTT,
  - `offline` as the Last Will message.
- Measurements are sent every second.
- Wi-Fi and MQTT connections are automatically re-established if lost.
- Sea level pressure is set to 1015.0 hPa.
