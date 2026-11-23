
# Hardware Components

This document lists the major hardware components used in the Intelligent Battery Management System prototype.

| Component | Quantity | Purpose |
|---|---:|---|
| STM32F401RE Nucleo | 1 | Main embedded controller |
| ESP32 Development Board | 1 | Wi-Fi and IoT communication |
| Cell voltage sensors | 2 | Measurement of two cell voltages |
| Potentiometer | 1 | Simulation of the third cell voltage input |
| ACS712 current sensor | 1 | Battery current measurement |
| LM35 temperature sensor | As required | Temperature measurement |
| OLED display | 1 | Local display of system parameters |
| Cooling fan | 1 | Temperature-based cooling |
| Buzzer | 1 | Fault indication |
| MOSFET/driver circuitry | As required | Control of external loads |
| Resistors and connecting wires | As required | Signal conditioning and connections |

## Controller

**STM32F401RE Nucleo**
- Main processing unit
- Performs ADC-based data acquisition
- Controls monitoring and protection functions
- Communicates with ESP32 through UART

## IoT Module

**ESP32**
- Receives telemetry from STM32
- Connects to a Wi-Fi network
- Uploads monitoring data to ThingSpeak

## Sensor Inputs

- Voltage sensors measure two cell-voltage inputs.
- A potentiometer simulates the third cell-voltage input.
- ACS712 measures current.
- LM35 provides temperature measurements.

## Output Devices

- OLED display for local monitoring
- Cooling fan for temperature control
- Buzzer for fault alerts

**Note:** The potentiometer is used to simulate a sensor input during prototype testing. It is not an actual cell voltage measurement.
