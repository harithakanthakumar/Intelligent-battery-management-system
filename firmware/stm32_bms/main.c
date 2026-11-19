#include "stm32f401xe.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* ==================================================
   STM32F401RE BMS
   USART1:
   PA9  = TX -> ESP32 GPIO16 RX
   PCLK2 = 16 MHz
   Baud = 115200

   OLED:
   PB6 = I2C1_SCL
   PB7 = I2C1_SDA
   Address = 0x3C
   ================================================== */

#define OLED_ADDR 0x3C
#define CELL_UV_LIMIT       3.0f
#define CELL_OV_LIMIT       4.2f

#define FAN_ON_TEMP         40.0f
#define FAN_OFF_TEMP        38.0f

#define TEMP_FAULT_LIMIT    50.0f
#define CHARGE_PIN 12U

/* ---------------- DELAY ---------------- */

void delay(volatile uint32_t count)
{
    while(count--)
        __NOP();
}

/* ==================================================
   USART1
   ================================================== */

void USART1_Init(void)
{
    RCC->AHB1ENR |= (1U << 0);
    RCC->APB2ENR |= (1U << 4);

    /* PA9 alternate function */
    GPIOA->MODER &= ~(3U << 18);
    GPIOA->MODER |=  (2U << 18);

    /* AF7 = USART1_TX */
    GPIOA->AFR[1] &= ~(15U << 4);
    GPIOA->AFR[1] |=  (7U << 4);

    USART1->CR1 = 0;
    USART1->CR2 = 0;
    USART1->CR3 = 0;

    /* PCLK2 = 16 MHz */
    USART1->BRR = 0x8B;

    /* Enable transmitter */
    USART1->CR1 |= (1U << 3);

    /* Enable USART1 */
    USART1->CR1 |= (1U << 13);
}

void USART1_SendChar(char c)
{
    while(!(USART1->SR & (1U << 7)));

    USART1->DR = (uint8_t)c;
}

void USART1_SendString(const char *str)
{
    while(*str)
        USART1_SendChar(*str++);

    while(!(USART1->SR & (1U << 6)));
}

/* ==================================================
   I2C1 INITIALIZATION
   PB6 = SCL
   PB7 = SDA
   ================================================== */

void OLED_I2C_Init(void)
{
    RCC->AHB1ENR |= (1U << 1);
    RCC->APB1ENR |= (1U << 21);

    GPIOB->MODER &= ~((3U << 12) | (3U << 14));
    GPIOB->MODER |=  ((2U << 12) | (2U << 14));

    GPIOB->OTYPER |= (1U << 6) | (1U << 7);

    GPIOB->OSPEEDR |= (3U << 12) | (3U << 14);

    GPIOB->AFR[0] &= ~((15U << 24) | (15U << 28));
    GPIOB->AFR[0] |=  ((4U << 24) | (4U << 28));

    RCC->APB1RSTR |= (1U << 21);
    RCC->APB1RSTR &= ~(1U << 21);

    I2C1->CR1 = 0;

    /* PCLK1 = 8 MHz */
    I2C1->CR2 = 8;

    /* 100 kHz */
    I2C1->CCR = 40;
    I2C1->TRISE = 9;

    I2C1->CR1 |= (1U << 0);
}

/* ==================================================
   I2C WRITE
   ================================================== */

int I2C_Write(uint8_t control,
              const uint8_t *data,
              uint16_t length)
{
    uint32_t timeout;

    timeout = 100000;

    while(I2C1->SR2 & (1U << 1))
    {
        if(--timeout == 0)
            return 0;
    }

    I2C1->CR1 |= (1U << 8);

    timeout = 100000;

    while(!(I2C1->SR1 & (1U << 0)))
    {
        if(--timeout == 0)
            goto error;
    }

    I2C1->DR = (OLED_ADDR << 1);

    timeout = 100000;

    while(!(I2C1->SR1 & (1U << 1)))
    {
        if(I2C1->SR1 & (1U << 10))
            goto error;

        if(--timeout == 0)
            goto error;
    }

    (void)I2C1->SR1;
    (void)I2C1->SR2;

    timeout = 100000;

    while(!(I2C1->SR1 & (1U << 7)))
    {
        if(--timeout == 0)
            goto error;
    }

    I2C1->DR = control;

    for(uint16_t i = 0; i < length; i++)
    {
        timeout = 100000;

        while(!(I2C1->SR1 & (1U << 7)))
        {
            if(I2C1->SR1 & (1U << 10))
                goto error;

            if(--timeout == 0)
                goto error;
        }

        I2C1->DR = data[i];
    }

    timeout = 100000;

    while(!(I2C1->SR1 & (1U << 2)))
    {
        if(--timeout == 0)
            goto error;
    }

    I2C1->CR1 |= (1U << 9);

    return 1;

error:

    I2C1->SR1 &= ~(1U << 10);
    I2C1->CR1 |= (1U << 9);

    return 0;
}

/* ==================================================
   OLED COMMAND / DATA
   ================================================== */

void OLED_Command(uint8_t command)
{
    I2C_Write(0x00, &command, 1);
}

void OLED_Data(const uint8_t *data, uint16_t length)
{
    I2C_Write(0x40, data, length);
}

/* ==================================================
   OLED INITIALIZATION
   ================================================== */

int OLED_Init(void)
{
    const uint8_t init_sequence[] =
    {
        0xAE,
        0xD5, 0x80,
        0xA8, 0x3F,
        0xD3, 0x00,
        0x40,
        0x8D, 0x14,
        0x20, 0x02,
        0xA1,
        0xC8,
        0xDA, 0x12,
        0x81, 0x7F,
        0xD9, 0xF1,
        0xDB, 0x40,
        0xA4,
        0xA6,
        0xAF
    };

    delay(100000);

    return I2C_Write(
        0x00,
        init_sequence,
        sizeof(init_sequence)
    );
}

void OLED_Clear(void)
{
    uint8_t zeros[128] = {0};

    for(uint8_t page = 0; page < 8; page++)
    {
        OLED_Command(0xB0 | page);
        OLED_Command(0x00);
        OLED_Command(0x10);

        OLED_Data(zeros, 128);
    }
}

void OLED_SetCursor(uint8_t page, uint8_t column)
{
    OLED_Command(0xB0 | page);
    OLED_Command(0x00 | (column & 0x0F));
    OLED_Command(0x10 | (column >> 4));
}

/* ==================================================
   5x7 FONT
   ================================================== */

const uint8_t font5x7[][5] =
{
    {0x00,0x00,0x00,0x00,0x00}, // space
    {0x00,0x00,0x5F,0x00,0x00}, // !
    {0x00,0x06,0x09,0x09,0x06}, // "
    {0x14,0x7F,0x14,0x7F,0x14}, // #
    {0x24,0x2A,0x7F,0x2A,0x12}, // $
    {0x23,0x13,0x08,0x64,0x62}, // %
    {0x36,0x49,0x55,0x22,0x50}, // &
    {0x00,0x05,0x03,0x00,0x00}, // '
    {0x00,0x1C,0x22,0x41,0x00}, // (
    {0x00,0x41,0x22,0x1C,0x00}, // )
    {0x14,0x08,0x3E,0x08,0x14}, // *
    {0x08,0x08,0x3E,0x08,0x08}, // +
    {0x00,0x50,0x30,0x00,0x00}, // ,
    {0x08,0x08,0x08,0x08,0x08}, // -
    {0x00,0x60,0x60,0x00,0x00}, // .
    {0x20,0x10,0x08,0x04,0x02}, // /
    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9
    {0x00,0x36,0x36,0x00,0x00}, // :
    {0x00,0x56,0x36,0x00,0x00}, // ;
    {0x08,0x14,0x22,0x41,0x00}, // <
    {0x14,0x14,0x14,0x14,0x14}, // =
    {0x00,0x41,0x22,0x14,0x08}, // >
    {0x02,0x01,0x51,0x09,0x06}, // ?
    {0x32,0x49,0x79,0x41,0x3E}, // @
    {0x7E,0x11,0x11,0x11,0x7E}, // A
    {0x7F,0x49,0x49,0x49,0x36}, // B
    {0x3E,0x41,0x41,0x41,0x22}, // C
    {0x7F,0x41,0x41,0x22,0x1C}, // D
    {0x7F,0x49,0x49,0x49,0x41}, // E
    {0x7F,0x09,0x09,0x09,0x01}, // F
    {0x3E,0x41,0x49,0x49,0x7A}, // G
    {0x7F,0x08,0x08,0x08,0x7F}, // H
    {0x00,0x41,0x7F,0x41,0x00}, // I
    {0x20,0x40,0x41,0x3F,0x01}, // J
    {0x7F,0x08,0x14,0x22,0x41}, // K
    {0x7F,0x40,0x40,0x40,0x40}, // L
    {0x7F,0x02,0x0C,0x02,0x7F}, // M
    {0x7F,0x04,0x08,0x10,0x7F}, // N
    {0x3E,0x41,0x41,0x41,0x3E}, // O
    {0x7F,0x09,0x09,0x09,0x06}, // P
    {0x3E,0x41,0x51,0x21,0x5E}, // Q
    {0x7F,0x09,0x19,0x29,0x46}, // R
    {0x46,0x49,0x49,0x49,0x31}, // S
    {0x01,0x01,0x7F,0x01,0x01}, // T
    {0x3F,0x40,0x40,0x40,0x3F}, // U
    {0x1F,0x20,0x40,0x20,0x1F}, // V
    {0x3F,0x40,0x38,0x40,0x3F}, // W
    {0x63,0x14,0x08,0x14,0x63}, // X
    {0x07,0x08,0x70,0x08,0x07}, // Y
    {0x61,0x51,0x49,0x45,0x43}, // Z
};

/* Print basic uppercase letters, numbers and punctuation */
void OLED_PrintChar(char c)
{
    uint8_t index;

    if(c >= ' ' && c <= 'Z')
        index = c - ' ';
    else
        index = 0;

    OLED_Data(font5x7[index], 5);

    uint8_t space = 0x00;
    OLED_Data(&space, 1);
}

void OLED_PrintString(const char *str)
{
    while(*str)
    {
        OLED_PrintChar(*str++);
    }
}

/* ==================================================
   ADC1 - STM32F401RE
   PA0 = ADC1_IN0  -> Voltage simulation potentiometer
   PB0 = ADC1_IN8  -> Temperature simulation potentiometer
   PB1 = ADC1_IN9  -> LM35
   ================================================== */

#define ADC_MAX_VALUE 4095.0f
#define ADC_VREF      3.3f

void ADC1_Init(void)
{
    /* Enable GPIOA and GPIOB clocks */
    RCC->AHB1ENR |= (1U << 0) | (1U << 1);

    /* Enable ADC1 clock */
    RCC->APB2ENR |= (1U << 8);

    /* PA0: Analog mode */
    GPIOA->MODER &= ~(3U << 0);
    GPIOA->MODER |=  (3U << 0);

    /* PB0 and PB1: Analog mode */
    GPIOB->MODER &= ~((3U << 0) | (3U << 2));
    GPIOB->MODER |=  ((3U << 0) | (3U << 2));

    /* No pull-up or pull-down */
    GPIOA->PUPDR &= ~(3U << 0);
    GPIOB->PUPDR &= ~((3U << 0) | (3U << 2));

    /* ADC common prescaler: PCLK2 / 2 */
    ADC->CCR &= ~(3U << 16);

    /* ADC1: single conversion, right aligned */
    ADC1->CR1 = 0;
    ADC1->CR2 = 0;

    /* Enable ADC */
    ADC1->CR2 |= (1U << 0);

    delay(10000);
}


/* Read one ADC channel */

uint16_t ADC_Read(uint8_t channel)
{
    /* One conversion in regular sequence */
    ADC1->SQR1 = 0;

    /* Select channel */
    ADC1->SQR3 = channel;

    /* Sample time: 84 ADC cycles */
    if(channel <= 9)
    {
        ADC1->SMPR2 &= ~(7U << (channel * 3));
        ADC1->SMPR2 |=  (4U << (channel * 3));
    }
    else
    {
        ADC1->SMPR1 &= ~(7U << ((channel - 10) * 3));
        ADC1->SMPR1 |=  (4U << ((channel - 10) * 3));
    }

    /* Start conversion */
    ADC1->CR2 |= (1U << 30);

    /* Wait until conversion complete */
    while(!(ADC1->SR & (1U << 1)));

    /* Read result */
    uint16_t result = (uint16_t)ADC1->DR;

    return result;
}


/* Convert ADC value to voltage */

float ADC_ToVoltage(uint16_t adc_value)
{
    return ((float)adc_value * ADC_VREF) / ADC_MAX_VALUE;
}

/* ==================================================
   FAN CONTROL - IRLZ44N
   PA6 -> MOSFET Gate
   ================================================== */

#define FAN_PIN          6U
#define FAN_ON_TEMP      40.0f
#define FAN_OFF_TEMP     38.0f

void Fan_Init(void)
{
    /* Enable GPIOA clock */
    RCC->AHB1ENR |= (1U << 0);

    /* PA6 as GPIO output */
    GPIOA->MODER &= ~(3U << (FAN_PIN * 2));
    GPIOA->MODER |=  (1U << (FAN_PIN * 2));

    /* Push-pull output */
    GPIOA->OTYPER &= ~(1U << FAN_PIN);

    /* Low speed */
    GPIOA->OSPEEDR &= ~(3U << (FAN_PIN * 2));

    /* No internal pull-up or pull-down */
    GPIOA->PUPDR &= ~(3U << (FAN_PIN * 2));

    /* Fan OFF initially */
    GPIOA->BSRR = (1U << (FAN_PIN + 16));
}

void Fan_ON(void)
{
    GPIOA->BSRR = (1U << FAN_PIN);
}

void Fan_OFF(void)
{
    GPIOA->BSRR = (1U << (FAN_PIN + 16));
}

#define BUZZER_PIN 13

void Buzzer_Init(void)
{
    /* Enable GPIOB clock */
    RCC->AHB1ENR |= (1U << 1);

    /* PB13 as output */
    GPIOB->MODER &= ~(3U << (BUZZER_PIN * 2));
    GPIOB->MODER |=  (1U << (BUZZER_PIN * 2));

    /* Push-pull output */
    GPIOB->OTYPER &= ~(1U << BUZZER_PIN);

    /* Low speed */
    GPIOB->OSPEEDR &= ~(3U << (BUZZER_PIN * 2));

    /* No pull-up or pull-down */
    GPIOB->PUPDR &= ~(3U << (BUZZER_PIN * 2));

    /* Buzzer OFF initially */
    GPIOB->BSRR = (1U << (BUZZER_PIN + 16));
}

void Buzzer_ON(void)
{
    GPIOB->BSRR = (1U << BUZZER_PIN);
}

void Buzzer_OFF(void)
{
    GPIOB->BSRR = (1U << (BUZZER_PIN + 16));
}

#define CHARGE_PIN 12U

void Charge_Init(void)
{
    RCC->AHB1ENR |= (1U << 1);  // Enable GPIOB clock

    // PB12 as GPIO output
    GPIOB->MODER &= ~(3U << (CHARGE_PIN * 2));
    GPIOB->MODER |=  (1U << (CHARGE_PIN * 2));

    // Push-pull output
    GPIOB->OTYPER &= ~(1U << CHARGE_PIN);

    // Low speed
    GPIOB->OSPEEDR &= ~(3U << (CHARGE_PIN * 2));

    // No pull-up or pull-down
    GPIOB->PUPDR &= ~(3U << (CHARGE_PIN * 2));

    // Charging control OFF initially
    GPIOB->BSRR = (1U << (CHARGE_PIN + 16));
}

void Charge_ON(void)
{
    GPIOB->BSRR = (1U << CHARGE_PIN);
}

void Charge_OFF(void)
{
    GPIOB->BSRR = (1U << (CHARGE_PIN + 16));
}


/* ==================================================
   MAIN
   ================================================== */

int main(void)
{
    USART1_Init();

    OLED_I2C_Init();

    Buzzer_Init();

    Charge_Init();

    if(OLED_Init())
    {
        OLED_Clear();
    }

    ADC1_Init();
    Fan_Init();

    uint16_t adc_voltage_pot;
    uint16_t adc_temp_pot;
    uint16_t adc_lm35;

    float voltage_pot;
    float temp_pot_voltage;
    float lm35_voltage;

    float simulated_cell_voltage;
    float simulated_temperature;
    float actual_temperature;

    char uart_message[120];
    char display_message[30];

    volatile unsigned long fault = 0;

    volatile uint8_t undervoltage_fault = 0;
    volatile uint8_t overvoltage_fault  = 0;
    volatile uint8_t overtemperature_fault = 0;


    volatile uint8_t fan_status = 0;

    while(1)
    {
        /* =====================================
           READ ADC CHANNELS
           ===================================== */

        /* PA0 = ADC channel 0 */
        adc_voltage_pot = ADC_Read(0);

        /* PB0 = ADC channel 8 */
        adc_temp_pot = ADC_Read(8);

        /* PB1 = ADC channel 9 */
        adc_lm35 = ADC_Read(9);

        uint16_t adc_cell2 = ADC_Read(1);  // PA1
        uint16_t adc_cell3 = ADC_Read(4);  // PA4

        float adc_voltage_cell2 = ADC_ToVoltage(adc_cell2);
        float adc_voltage_cell3 = ADC_ToVoltage(adc_cell3);




        /* =====================================
           CONVERT ADC TO VOLTAGE
           ===================================== */

        voltage_pot = ADC_ToVoltage(adc_voltage_pot);

        temp_pot_voltage = ADC_ToVoltage(adc_temp_pot);

        lm35_voltage = ADC_ToVoltage(adc_lm35);


        /* =====================================
           CALCULATE SIMULATED VALUES
           ===================================== */

        /*
           Voltage potentiometer:
           Map 0-3.3 V potentiometer output
           to a simulated 0-4.2 V cell voltage.
        */

        simulated_cell_voltage =
            (voltage_pot / ADC_VREF) * 4.2f;


        /*
           Temperature potentiometer:
           Simulate LM35 output voltage.
           LM35 scale = 10 mV per degree C.
        */

        simulated_temperature =
            temp_pot_voltage / 0.010f;

        /* =====================================
           FAN CONTROL USING SIMULATED TEMP
           ===================================== */

        static uint8_t fan_status = 0;

        /* Fan ON at 40 C */
        if (simulated_temperature >= FAN_ON_TEMP)
        {
            fan_status = 1;
        }

        /* Fan OFF at 38 C */
        else if (simulated_temperature <= FAN_OFF_TEMP)
        {
            fan_status = 0;
        }

        /* Control the MOSFET */
        if (fan_status)
        {
            Fan_ON();
        }
        else
        {
            Fan_OFF();
        }

        /*
         * Buzzer control
         */
        if (fault)
        {
            Buzzer_ON();
        }
        else
        {
            Buzzer_OFF();
        }


        /*
           Actual LM35:
           Temperature = Vout / 0.010
        */

        actual_temperature =
            lm35_voltage / 0.010f;

        /* Undervoltage fault */
        undervoltage_fault = 0;

        if ((simulated_cell_voltage < CELL_UV_LIMIT) ||
            (adc_voltage_cell2 < CELL_UV_LIMIT) ||
            (adc_voltage_cell3 < CELL_UV_LIMIT))
        {
            undervoltage_fault = 1;
        }

        /* Overvoltage fault */
        overvoltage_fault = 0;

        if ((simulated_cell_voltage > CELL_OV_LIMIT) ||
            (adc_voltage_cell2 > CELL_OV_LIMIT) ||
            (adc_voltage_cell3 > CELL_OV_LIMIT))
        {
            overvoltage_fault = 1;
        }

        /* Overtemperature fault */
        overtemperature_fault = 0;

        if (actual_temperature > TEMP_FAULT_LIMIT)
        {
            overtemperature_fault = 1;
        }

        /* Overall fault */
        fault = undervoltage_fault ||
                overvoltage_fault ||
                overtemperature_fault;

        if (undervoltage_fault ||
            overvoltage_fault ||
            overtemperature_fault)
        {
            Buzzer_ON();x
        }
        else
        {
            Buzzer_OFF();
        }

        if ((simulated_cell_voltage < 4.2f) &&
            (adc_voltage_cell2 < 4.2f) &&
            (adc_voltage_cell3 < 4.2f) &&
            (actual_temperature < 50.0f) &&
            (!fault))
        {
            Charge_ON();
        }
        else
        {
            Charge_OFF();
        }


        /* =====================================
           SEND VALUES TO ESP32
           ===================================== */

        snprintf(
            uart_message,
            sizeof(uart_message),
            "C1:%.2f,C2:%.2f,C3:%.2f,T1:%.1f,T2:%.2f,I:%.2f,Fault:%lu,Fan:%u\r\n",

            simulated_cell_voltage,
            adc_voltage_cell2,
            adc_voltage_cell3,
            actual_temperature,
            simulated_temperature,
			0.0f,fault,
			(unsigned int)fan_status


        );

        USART1_SendString(uart_message);


        /* =========================================
           OLED DISPLAY - BMS MONITOR
           ========================================= */

        OLED_Clear();

        OLED_SetCursor(0, 0);
        OLED_PrintString("BMS MONITOR");

        /* Cell voltages */
        OLED_SetCursor(1, 0);
        snprintf(display_message, sizeof(display_message),
                 "C1:%.2fV C2:%.2fV", simulated_cell_voltage, adc_voltage_cell2);
        OLED_PrintString(display_message);

        OLED_SetCursor(2, 0);
        snprintf(display_message, sizeof(display_message),
                 "C3:%.2fV",adc_voltage_cell3);
        OLED_PrintString(display_message);

        /* Original LM35 temperature */
        OLED_SetCursor(3, 0);
        snprintf(display_message, sizeof(display_message),
                 "LM35: %.1f C", actual_temperature);
        OLED_PrintString(display_message);

        /* Simulated temperature */
        OLED_SetCursor(4, 0);
        snprintf(display_message, sizeof(display_message),
                 "SIM T: %.1f C", simulated_temperature);
        OLED_PrintString(display_message);

        /* Fault status */
        OLED_SetCursor(5, 0);

        if (undervoltage_fault)
        {
            OLED_PrintString("FAULT: UNDER VOLT");
        }
        else if (overvoltage_fault)
        {
            OLED_PrintString("FAULT: OVER VOLT");
        }
        else if (overtemperature_fault)
        {
            OLED_PrintString("FAULT: OVER TEMP");
        }
        else
        {
            OLED_PrintString("FAULT: NORMAL");
        }

        /* Fan status */
        OLED_SetCursor(6, 0);

        if (fan_status)
        {
            OLED_PrintString("FAN: ON");
        }
        else
        {
            OLED_PrintString("FAN: OFF");
        }

        delay(800000);
    }
}
