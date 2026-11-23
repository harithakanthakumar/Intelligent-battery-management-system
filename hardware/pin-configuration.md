# Pin configuration

Verified against the active STM32F401RE bare-metal BMS firmware. Unused peripherals (USART2, Servo, Balancing relays, INA219, and status LEDs) have been removed.

| STM32 pin | Peripheral / mode | Intended function | Verified? |
|---|---|---|---|
| PA0 | ADC1_IN0 | Cell 1 voltage input (potentiometer simulation) | Yes |
| PA1 | ADC1_IN1 | Cell 2 voltage input | Yes |
| PA4 | ADC1_IN4 | Cell 3 voltage input | Yes |
| PA6 | GPIO_Output | Cooling fan MOSFET control (IRLZ44N) | Yes |
| PA9 | USART1_TX | Telemetry UART TX to ESP32 (GPIO16 RX) @ 115200 baud | Yes |
| PB0 | ADC1_IN8 | Temperature 1 input (potentiometer simulation) | Yes |
| PB1 | ADC1_IN9 | Temperature 2 input (LM35 sensor) | Yes |
| PB6 | I2C1_SCL | OLED display I2C clock (100 kHz) | Yes |
| PB7 | I2C1_SDA | OLED display I2C data (Addr: 0x3C) | Yes |
| PB12 | GPIO_Output | Charge control FET gate | Yes |
| PB13 | GPIO_Output | Fault alarm buzzer | Yes |
