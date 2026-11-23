
# STM32 BMS Firmware

## Overview

This directory contains the firmware for the STM32F401RE microcontroller, which acts as the primary controller of the Intelligent Battery Management System.

The firmware is developed using Embedded C and bare-metal register-level programming.

## Main Functions

- ADC initialization and sensor data acquisition
- Cell voltage monitoring
- Current and temperature monitoring
- Fault detection
- Fan control
- Buzzer control
- OLED display updates
- UART communication with ESP32

## Peripherals Used

| Peripheral | Application |
|---|---|
| ADC | Analog sensor measurements |
| USART1 | Telemetry transmission to ESP32 |
| I2C1 | OLED communication |
| GPIO | Control of external devices |
| Timer/PWM | Actuator control, where configured |

## Source File

- `main.c` – Main STM32 firmware.

## Development Environment

- Microcontroller: STM32F401RE
- Board: Nucleo-F401RE
- Language: Embedded C
- Programming approach: Bare-metal register-level programming

## Notes

Verify sensor scaling, ADC reference voltage, GPIO configuration, and actuator driver connections before hardware testing.
