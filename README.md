# Intelligent Battery Management System

An STM32-based Battery Management System developed across the **Battery Management Systems (BMS)** and **Embedded Systems with ARM** courses.

> **Project status:** Replace the placeholders in this repository with the final, verified implementation and test results. Features that are planned or simulated must be labelled accordingly.

## Overview

The project explores battery parameter monitoring and control using an STM32F401RE (ARM Cortex-M4) as the main controller, with an ESP32 providing wireless connectivity and cloud visualization.

The STM32 firmware is intended to demonstrate low-level embedded programming and peripheral interfacing. The ESP32 handles the IoT communication path.

## System architecture

```text
Cell-voltage inputs ─┐
Current sensor ──────┼──> STM32F401RE ── UART ──> ESP32 ── Wi-Fi ──> ThingSpeak
Temperature inputs ──┘       │
                             ├── Fan / actuator control
                             └── Fault indication
```

Confirm this diagram against the final hardware before submission.

## Main functions

- Cell-voltage monitoring
- Battery-current measurement
- Temperature monitoring
- Fault indication and protection logic
- Fan control based on the implemented temperature threshold
- STM32-to-ESP32 UART communication
- Cloud visualization through ThingSpeak

Only describe a function as implemented after verifying it in the final code and hardware tests. Clearly identify simulated sensor inputs.

## Hardware

Update this list to match the final build:

- STM32F401RE Nucleo board
- ESP32 development board
- ACS712 current sensor
- Two cell-voltage sensor inputs
- Potentiometer input used to simulate a third cell voltage (if retained)
- LM35 temperature sensor(s)
- Potentiometer-based temperature simulation (if retained)
- MOSFET switching stage, fan and buzzer (as applicable)

## Software and tools

- Embedded C
- STM32CubeIDE / GNU Arm toolchain (confirm exact toolchain)
- CMSIS device definitions (if used by the firmware)
- ESP32 development environment (specify Arduino core or ESP-IDF)
- ThingSpeak

## Repository structure

```text
firmware/
  stm32_bms/       STM32 firmware and project files
  esp32_iot/       ESP32 communication and cloud code
hardware/          Pin map, wiring and component information
docs/              Architecture and implementation notes
results/           Verified test evidence and photographs
```

## Build and flash

Build and flash instructions are intentionally left as a checklist until the exact project files and toolchain configuration are added.

1. Install the required STM32 toolchain and board support.
2. Open the STM32 project from `firmware/stm32_bms/`.
3. Check the board, clock configuration, pin mapping and sensor scaling.
4. Build the project and resolve any warnings or errors.
5. Flash the Nucleo board and verify serial/debug output.
6. Configure the ESP32 credentials locally and flash its firmware.
7. Verify UART communication and cloud updates.

Do not commit Wi-Fi passwords, API keys or access tokens.

## Documentation

- [Project overview](docs/project-overview.md)
- [Hardware and components](hardware/components.md)
- [Pin configuration](hardware/pin-configuration.md)
- [Connections and safety](hardware/connections.md)
- [Testing and results](results/README.md)

## Limitations and future work

Document limitations honestly. Possible future work may include improved state-of-charge estimation, state-of-health estimation, predictive imbalance detection and more advanced balancing. These are not claimed as implemented unless supported by the final firmware and test evidence.

## Team

Add team members, roles and course details here.

## License

See [LICENSE](LICENSE).
