
# Project Overview

## Project Title

Intelligent Battery Management System

## Problem Statement

Battery-powered systems require continuous monitoring to maintain safe operating conditions and reliable performance. Variations in cell voltage, current, and temperature can affect battery operation and may lead to unsafe conditions if not detected.

This project focuses on developing an embedded battery monitoring and protection system capable of acquiring battery parameters, identifying abnormal operating conditions, and communicating monitoring data remotely.

## Proposed Solution

The proposed system uses an STM32F401RE microcontroller as the primary processing unit and an ESP32 for wireless communication.

The STM32 collects sensor readings through its ADC peripheral, processes the measurements, performs fault checks, and controls external indicators and actuators. The ESP32 receives the processed data through UART and uploads it to ThingSpeak for remote monitoring.

## Main Objectives

1. Monitor individual cell voltages.
2. Measure battery current and temperature.
3. Detect abnormal operating conditions.
4. Implement basic protection and alert mechanisms.
5. Display battery parameters locally.
6. Enable remote monitoring through IoT communication.

## Controller Responsibilities

### STM32F401RE

- ADC configuration and data acquisition
- Sensor interfacing
- Voltage, current, and temperature monitoring
- Fault detection
- Fan and buzzer control
- OLED communication through I2C
- UART data transmission

### ESP32

- UART data reception
- Wi-Fi connectivity
- HTTP communication
- ThingSpeak data upload

## Development Focus

The project emphasizes:
- Bare-metal STM32 programming
- Register-level peripheral configuration
- Embedded C development
- Sensor interfacing
- Serial communication
- IoT integration

## Future Scope

The system can be extended with battery State of Charge estimation, State of Health estimation, predictive cell imbalance detection, and battery lifecycle analysis.
