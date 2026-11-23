
# ESP32 IoT Firmware

## Overview

This directory contains the ESP32 firmware responsible for wireless communication and cloud-based monitoring.

The ESP32 receives battery telemetry from the STM32F401RE through UART and uploads the received information to the ThingSpeak IoT platform using Wi-Fi.

## Main Functions

- UART communication with STM32
- Wi-Fi connection
- Reception of battery telemetry
- HTTP communication
- ThingSpeak data upload

## Communication

| Interface | Purpose |
|---|---|
| UART2 | Receives telemetry from STM32 |
| Wi-Fi | Connects to the network |
| HTTP | Sends data to ThingSpeak |

## Source Files

- `main.cpp` – ESP32 Arduino firmware.
- `secrets.example.h` – Example configuration file containing placeholder credentials.

## Development Environment

- Board: ESP32 Development Board
- Framework: Arduino
- Language: C++
- Cloud Platform: ThingSpeak

## Security

Do not upload actual Wi-Fi passwords or ThingSpeak API keys to GitHub.

Create a local `secrets.h` file containing your credentials and exclude it using `.gitignore`.
