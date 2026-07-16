# High-Altitude Weather Balloon Telemetry System

Personal engineering project focused on designing and prototyping a low-cost telemetry payload for high-altitude weather balloon testing. The system collects GPS and environmental data, logs flight data to a CSV file, transmits live telemetry using LoRa, and displays real-time updates through a web/app dashboard.

## Project Overview

The goal of this project is to build a student-designed telemetry system capable of supporting future high-altitude balloon launches. The payload is intended to provide both onboard data logging and live wireless updates so that position, altitude, and environmental conditions can be monitored during testing and flight.

This project combines mechanical design, embedded systems, sensor integration, wireless communication, and data visualization.

## Features

- GPS position tracking
- Altitude, speed, and satellite data collection
- Temperature, pressure, and humidity sensing using a BME280 sensor
- LoRa wireless telemetry transmission
- SD card CSV data logging
- Live dashboard/web/app updates for real-time data visualization
- Serial monitor output for ground testing and debugging

## Data Collected

The system is designed to collect and log the following data:

| Data Type | Description |
|---|---|
| Time | Timestamp or loop count for each reading |
| Latitude | GPS latitude |
| Longitude | GPS longitude |
| GPS Altitude | Altitude from GPS module |
| BME Altitude | Altitude estimated from pressure sensor |
| Temperature | Ambient temperature from BME280 |
| Pressure | Atmospheric pressure from BME280 |
| Humidity | Relative humidity from BME280 |
| Speed | GPS-based speed |
| Satellites | Number of GPS satellites connected |

## Hardware Used

- Arduino-compatible microcontroller
- GPS module
- BME280 temperature, pressure, and humidity sensor
- LoRa radio module
- SD card module
- MicroSD card
- Battery power supply
- Jumper wires / soldered connections
- 3D printed payload housing

## Software Used

- Arduino IDE
- C/C++
- HTML

## Repository Structure

```text
high-altitude-weather-balloon/
├── README.md
├── code/
│   └── weather_balloon_telemetry.ino
├── data/
│   └── sample_log.csv
├── images/
│   ├── prototype.jpg
│   ├── wiring.jpg
│   └── dashboard.png
└── docs/
    ├── bill-of-materials.md
    └── test-notes.md
