# InfluxDB

A short guide to setting up InfluxDB on Raspberry Pi.

---

## Installation

Install InfluxDB using the official repository:

```bash
sudo apt update
wget -q https://repos.influxdata.com/influxdata-archive.key

sudo mkdir -p /etc/apt/keyrings

gpg --dearmor influxdata-archive.key | sudo tee /etc/apt/keyrings/influxdata-archive.gpg > /dev/null

echo "deb [signed-by=/etc/apt/keyrings/influxdata-archive.gpg] https://repos.influxdata.com/debian stable main" \
| sudo tee /etc/apt/sources.list.d/influxdata.list

sudo apt update
sudo apt install influxdb2
```

## Starting InfluxDB

Enable and start InfluxDB service:

```bash
sudo systemctl enable influxdb
sudo systemctl start influxdb
sudo systemctl status influxdb
```

You should see:

`active (running)`

## Access

Open InfluxDB in your browser:

- `http://192.168.1.25:8086`

or locally:

- `http://localhost:8086`

## Initial Setup

On the first launch configure:
- `Username`
- `Password`
- `Organization` - smartHome
- `Bucket` - homeData

Save the generated API token - it will be required to connect Node-RED.

## Data Model

### Measurement

- `environment`

### Tags

- `room (office, kitchen, bathroom, livingroom)`

### Fields

- `temperature`
- `humidity`
- `pressure`
- `light`
- `altitude`

## Example Data (Line Protocol)

`environment,room=office temperature=24.84,humidity=32.53,pressure=999.72,light=785.83,altitude=127.74`

## Useful Commands

```bash
sudo systemctl restart influxdb
sudo systemctl stop influxdb
sudo systemctl status influxdb
```

## Notes
- Default InfluxDB port: `8086`
- Web UI is available after installation
- Used as a time-series database for sensor data
- Connected to Node-RED via API token
