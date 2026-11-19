
# Intelligent Battery Management System

### Battery Intelligence for Predictive Health, Safety, and Lifecycle Optimization

## Overview

The Intelligent Battery Management System (BMS) is an embedded systems project designed to monitor battery operating conditions, identify abnormal behaviour, and improve battery safety and reliability.

The system uses an **STM32F401RE microcontroller** as its primary controller for sensor interfacing, data acquisition, monitoring, and protection functions. An **ESP32** handles wireless communication and sends battery-related data to the ThingSpeak IoT platform.

The project focuses on bare-metal embedded programming, STM32 peripheral configuration, sensor interfacing, UART communication, and IoT integration.

## Key Features

- Individual cell voltage monitoring
- Battery current monitoring
- Temperature monitoring
- Fault detection and indication
- Temperature-based cooling fan control
- Buzzer-based fault alerts
- OLED-based local monitoring
- UART communication between STM32 and ESP32
- ThingSpeak-based remote monitoring

## System Architecture

The system is divided into two main sections:

### 1. STM32F401RE – Battery Monitoring and Control

The STM32 acts as the main controller. It performs:
- ADC-based sensor data acquisition
- Cell voltage monitoring
- Current and temperature monitoring
- Fault detection
- Cooling fan control
- Buzzer control
- OLED display updates
- UART data transmission

### 2. ESP32 – IoT Communication

The ESP32 receives battery telemetry from the STM32 through UART and uploads the data to ThingSpeak using Wi-Fi.

## Hardware Components

| Component | Function |
|---|---|
| STM32F401RE Nucleo | Main BMS controller |
| ESP32 | Wireless communication |
| Voltage sensors | Cell voltage measurement |
| ACS712 | Current measurement |
| LM35 | Temperature measurement |
| Potentiometer | Simulated sensor input |
| OLED Display | Local monitoring |
| Cooling Fan | Temperature-based cooling |
| Buzzer | Fault indication |

Refer to the `hardware/` directory for component details and pin configurations.

## Software and Technologies

- Embedded C
- STM32F401RE
- Bare-metal register-level programming
- ESP32 Arduino Framework
- ADC
- UART
- I2C
- Wi-Fi communication
- ThingSpeak IoT Platform

## Working Principle

1. Sensors provide battery-related measurements to the STM32.
2. The STM32 reads analog inputs using its ADC peripheral.
3. The controller processes the readings and checks for abnormal conditions.
4. Protection and indication outputs are activated when required.
5. Battery information is displayed on the OLED.
6. The STM32 transmits telemetry to the ESP32 through UART.
7. The ESP32 uploads the received data to ThingSpeak over Wi-Fi.

## Repository Structure

```text
Intelligent-battery-management-system/
│
├── docs/
│   ├── architecture.md
│   └── project-overview.md
│
├── firmware/
│   ├── stm32_bms/
│   │   ├── main.c
│   │   └── README.md
│   │
│   └── esp32_iot/
│       ├── main.cpp
│       ├── secrets.example.h
│       └── README.md
│
├── hardware/
│   ├── components.md
│   ├── connections.md
│   └── pin-configuration.md
│
├── results/
│   └── README.md
│
└── README.md
```

## Project Objectives

- Develop an embedded battery monitoring system.
- Configure STM32 peripherals using register-level programming.
- Monitor individual cell voltages and battery operating parameters.
- Implement fault detection and basic protection mechanisms.
- Establish communication between STM32 and ESP32.
- Enable remote battery monitoring using an IoT platform.

## Future Improvements

- State of Charge (SOC) estimation
- State of Health (SOH) estimation
- Predictive cell imbalance detection
- Remaining Useful Life (RUL) prediction
- Advanced cell balancing techniques
- Battery degradation analysis

## Applications

- Electric vehicles
- Battery energy storage systems
- Portable battery-powered systems
- Renewable energy storage

## Project Status

Embedded monitoring, protection, and IoT integration are the primary development areas. Features under development or proposed for future work will be documented separately from verified implementation results.

## Team

Developed as part of the Battery Management Systems and Embedded Systems with ARM coursework.
