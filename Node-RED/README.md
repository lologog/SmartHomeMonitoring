# Node-RED

A short guide to setting up Node-RED on Raspberry Pi.

## Installation

```bash
sudo apt update
sudo apt install -y build-essential git curl
bash <(curl -sL https://github.com/node-red/linux-installers/releases/latest/download/update-nodejs-and-nodered-deb)
```

## Starting Node-RED

```bash
node-red-start
sudo systemctl enable nodered.service
sudo systemctl start nodered.service
sudo systemctl status nodered.service
```

## Access

Open in browser:

```text
http://192.168.1.25:1880
```

or locally:

```text
http://localhost:1880
```

## Testing MQTT in Node-RED

Send test message:

```bash
mosquitto_pub -h localhost -t test/esp32 -m "Hello"
```

Receive (Node-RED debug panel):

```text
msg.payload : "Hello"
```

## Useful commands

```bash
sudo systemctl restart nodered
ps -aux | grep node-red
```

## Notes

* Default Node-RED port: `1880`
* Node-RED connects to MQTT broker on `localhost:1883`
* Use IP address to access Node-RED from other devices
