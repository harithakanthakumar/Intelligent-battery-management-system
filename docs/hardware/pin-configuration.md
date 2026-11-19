# Pin configuration

Fill this table from the final firmware and wiring. The entries below are a starting point from the project configuration notes; verify every pin before use.

| STM32 pin | Peripheral / mode | Intended function | Verified? |
|---|---|---|---|
| PA0 | ADC1_IN0 | Cell voltage input 1 | No |
| PA1 | ADC1_IN1 | Cell voltage input 2 | No |
| PA2 | USART2_TX | UART to ESP32 RX | No |
| PA3 | USART2_RX | UART from ESP32 TX | No |
| PA4 | ADC1_IN4 | Additional analog input, if used | No |
| PA6 | TIM3_CH1 | Fan PWM, if used | No |
| PA8 | TIM1_CH1 | Servo PWM, if retained | No |
| PA9 | USART1_TX | Debug output, if used | No |
| PA10 | USART1_RX | Debug input, if used | No |
| PB0 | ADC1_IN8 | Temperature analog input | No |
| PB1 | ADC1_IN9 | Additional analog input | No |
| PB6 | I2C1_SCL | I2C clock, if used | No |
| PB7 | I2C1_SDA | I2C data, if used | No |
| PB12 | GPIO output | Charge control, if retained | No |

Delete unused rows and mark a row verified only after checking the schematic and firmware.
