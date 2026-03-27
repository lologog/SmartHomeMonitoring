# MQTT_Broker

A short guide to setting up an MQTT broker using Mosquitto.

## Installation

```bash
sudo apt update
sudo apt install -y mosquitto mosquitto-clients
```

## Starting the broker

```bash
sudo systemctl enable mosquitto
sudo systemctl start mosquitto
sudo systemctl status mosquitto
```

## Testing

Publish a message:

```bash
mosquitto_pub -h localhost -t test/esp32 -m "Hello from Raspberry Pi"
```

Receive a message:

```bash
mosquitto_sub -h localhost -t test/esp32
```

## Testing with IP address

```bash
mosquitto_pub -h 192.168.1.25 -t test/esp32 -m "test via ip"
mosquitto_sub -h 192.168.1.25 -t test/esp32
```

## Useful commands

```bash
sudo systemctl restart mosquitto
ps -aux | grep mosquitto
```

## Notes

- The default MQTT port is `1883`
- `localhost` works locally on the Raspberry Pi
- use the IP address to connect from other devices in the same network
