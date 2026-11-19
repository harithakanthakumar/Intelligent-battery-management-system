# Hardware components

| Component | Quantity | Purpose | Status / notes |
|---|---:|---|---|
| STM32F401RE Nucleo | 1 | Main controller | Confirm exact board revision |
| ESP32 development board | 1 | Wi-Fi / IoT communication | Specify exact module |
| ACS712 | 1 | Current sensing | Confirm 5 A / 20 A / 30 A variant |
| Cell-voltage sensor inputs | 2 | Cell voltage measurement | Record divider ratios and safe input range |
| Potentiometer | As used | Simulated third-cell input, if retained | Label as simulation |
| LM35 | As used | Temperature measurement | Record sensor count and calibration |
| MOSFETs | As used | Switching / control | Add exact part numbers |
| Fan and buzzer | As used | Thermal control / fault alert | Record ratings |

Update quantities, part numbers and status to match the final build. Do not connect battery cells until the sensing and protection circuit has been checked for safe voltage ranges and common-ground requirements.
