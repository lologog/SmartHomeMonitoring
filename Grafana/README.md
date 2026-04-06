# Grafana

A short guide to setting up Grafana on Raspberry Pi.

## Installation

```bash
sudo apt update
sudo apt install -y apt-transport-https software-properties-common wget
sudo mkdir -p /etc/apt/keyrings/
wget -q -O - https://apt.grafana.com/gpg.key | sudo gpg --dearmor -o /etc/apt/keyrings/grafana.gpg

echo "deb [signed-by=/etc/apt/keyrings/grafana.gpg] https://apt.grafana.com stable main" | sudo tee /etc/apt/sources.list.d/grafana.list

sudo apt update
sudo apt install grafana
```

## Starting Grafana

```bash
sudo systemctl enable grafana-server
sudo systemctl start grafana-server
sudo systemctl status grafana-server
```

## Access

Open in browser:
- `http://192.168.1.25:3000`

or locally:
- `http://localhost:3000`

Default login:

- user: `admin`
- password: `admin`

## Connecting InfluxDB

1. Go to `Connections -> Data sources`
2. Add InfluxDB
3. Set:

- `URL: http://localhost:8086`

- `Query language: Flux`

- `Organization: smartHome`

- `Bucket: homeData`

4. Paste your InfluxDB token
5. Click `Save & Test`

## Importing Dashboard

1. Go to `Dashboards -> Import`
2. Upload `dashboard.json`
3. Select your InfluxDB datasource
4. Click `Import`

## Useful commands

```bash
sudo systemctl restart grafana-server
ps -aux | grep grafana
```

## Notes

- Default Grafana port: `3000`
- Grafana connects to InfluxDB on `localhost:8086`
- Data comes from Node-RED via MQTT -> InfluxDB pipeline
