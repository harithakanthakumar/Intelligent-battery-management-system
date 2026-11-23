# Connections and Electrical Safety

## 1. System Wiring Diagram

![STM32F401RE 3S Li-Ion BMS System Wiring Diagram](./hardware/wiring_diagram.png)

*(Ensure `wiring_diagram.png` is placed in the same directory as this `connections.md` file, or update the path above to match your repository structure.)*

---

## 2. Battery Cell Arrangement & Pack Voltage
* **Configuration:** 3S1P (3 Lithium-Ion cells connected in series).
* **Nominal Pack Voltage:** $3 \times 3.7\text{V} = 11.1\text{V}$
* **Minimum Cutoff Voltage (UV):** $3 \times 3.0\text{V} = 9.0\text{V}$ (`CELL_UV_LIMIT = 3.0f`)
* **Maximum Pack Voltage (OV):** $3 \times 4.2\text{V} = 12.6\text{V}$ (`CELL_OV_LIMIT = 4.2f`)

---

## 3. Voltage-Divider Resistors & ADC Input Limits
* **STM32F401RE ADC Limits:** Nominal $0\text{V} \text{ to } 3.3\text{V}$ ($V_{\text{DDA}}$). Exceeding $3.6\text{V}$ on non-5V tolerant analog pins will permanently damage the microcontroller.
* **Divider Formula:**
  $$V_{\text{ADC}} = V_{\text{tap}} \times \left(\frac{R_2}{R_1 + R_2}\right)$$
* *Firmware Note:* Individual upper cell voltages must be calculated in firmware by subtracting the lower tap potentials ($V_{\text{Cell2}} = V_{\text{B2}} - V_{\text{B1}}$ and $V_{\text{Cell3}} = V_{\text{B3}} - V_{\text{B2}}$).

| Tap / Input | Max Tap Voltage | $R_1$ (Top) | $R_2$ (Bottom) | Divider Ratio | Max ADC Voltage | STM32 Pin |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **Cell 1 ($B_1$)** *(or Sim Pot)* | $4.2\text{V}$ *(3.3V Pot)* | $10\text{k}\Omega$ | $10\text{k}\Omega$ | $0.500$ (1:2) | $2.10\text{V}$ | `PA0` (`ADC1_IN0`) |
| **Cell 2 ($B_2$)** | $8.4\text{V}$ | $20\text{k}\Omega$ | $10\text{k}\Omega$ | $0.333$ (1:3) | $2.80\text{V}$ | `PA1` (`ADC1_IN1`) |
| **Cell 3 ($B_3$ / Pack+)** | $12.6\text{V}$ | $30\text{k}\Omega$ | $10\text{k}\Omega$ | $0.250$ (1:4) | $3.15\text{V}$ | `PA4` (`ADC1_IN4`) |

* **Noise Filtering:** Connect a $100\text{nF}$ ceramic capacitor in parallel with each $R_2$ resistor to filter high-frequency switching noise and stabilize the ADC sample-and-hold capacitor.

---

## 4. Sensor Supply Voltages & Output Ranges

| Component | Supply Voltage | Output / Signal Range | STM32 Interface | Notes |
| :--- | :--- | :--- | :--- | :--- |
| **LM35 Temp Sensor** | $5.0\text{V}$ (`5V` rail) | $0\text{V} \text{ to } 1.50\text{V}$ ($10\text{mV}/^\circ\text{C}$) | `PB1` (`ADC1_IN9`) | $50^\circ\text{C}$ fault limit equals $0.50\text{V}$ (safe for $3.3\text{V}$ ADC). |
| **Temp Sim Potentiometer** | $3.3\text{V}$ (`3V3` rail) | $0\text{V} \text{ to } 3.30\text{V}$ | `PB0` (`ADC1_IN8`) | Must be powered from $3.3\text{V}$, never $5\text{V}$. |
| **0.96" SSD1306 OLED** | $3.3\text{V}$ (`3V3` rail) | $3.3\text{V}$ I2C Logic (100 kHz) | `PB6` (SCL), `PB7` (SDA) | Use $4.7\text{k}\Omega$ pull-up resistors to $3.3\text{V}$ on both lines. |
| **Active Buzzer** | $3.3\text{V} \text{ to } 5.0\text{V}$ | Digital On/Off | `PB13` (`GPIO_Output`) | Drive via an NPN transistor (`2N2222`) if buzzer current exceeds $20\text{mA}$. |

---

## 5. Ground & Reference Arrangement
* **Star Ground Topology:** Tie Battery `B-`, STM32 `GND`, ESP32 `GND`, 12V-to-5V Buck Converter `GND`, and both IRLZ44N MOSFET Source pins to a single central ground bus bar. This prevents high-current fan or charging paths from introducing ground-bounce offsets into millivolt LM35 temperature readings.
* **Logic Level Compatibility:** Both the STM32F401RE (`PA9` TX) and ESP32 (`GPIO16` RX) operate at **$3.3\text{V}$ TTL logic**—direct connection is safe without a level shifter.

---

## 6. MOSFET Gate-Driver Arrangement & Load Supply
* **MOSFET Selection:** Logic-level N-Channel MOSFETs (**IRLZ44N**), which achieve full saturation at $V_{\text{GS}} = 3.3\text{V}$ ($R_{\text{DS(on)}} \approx 0.025\Omega$).
* **Cooling Fan (`PA6`):**
  * **Load Supply:** Switched $+12.6\text{V}$ pack rail (after fuse and kill switch).
  * **Gate Drive:** `PA6` $\rightarrow$ $100\Omega$ series gate resistor $\rightarrow$ IRLZ44N Gate, with a $10\text{k}\Omega$ pull-down resistor from Gate to Star Ground so the fan defaults to **OFF** during MCU reset. Place a `1N4007` flyback diode across the fan terminals (cathode to $+12.6\text{V}$).
* **Charge Control (`PB12`):**
  * **Gate Drive:** `PB12` $\rightarrow$ $100\Omega$ series gate resistor $\rightarrow$ IRLZ44N Gate, with a $10\text{k}\Omega$ pull-down resistor to Star Ground to guarantee the charge path stays **OFF** on power-up or brownout.

---

## 7. Fuse, Current Limiting & Emergency Disconnect
* **Main Overcurrent Fuse:** Install a **$5\text{A}$ fast-blow fuse** directly in series with the positive pack terminal (`B+`) upstream of all branches, converters, and voltage dividers.
* **Emergency Disconnect:** Place a physical **high-current SPST toggle/kill switch** immediately after the $5\text{A}$ fuse to provide instant manual isolation of the battery pack.
* **Hardware Fail-Safe:** Always pair this controller with a dedicated **3S hardware BMS protection board** (e.g., S-8254A / DW01A based) wired directly across the cells for autonomous hardware over-charge, over-discharge, and short-circuit cutoff independent of STM32 firmware execution.

---

## 8. Mandatory Pre-Power Safety Checklist
1. **Never** connect Cell 2 ($8.4\text{V}$) or Cell 3 ($12.6\text{V}$) directly to any STM32 pin without verified voltage dividers installed.
2. Before inserting the STM32 into its headers, power on the battery pack and verify with a multimeter that the voltages at `PA0`, `PA1`, and `PA4` are all **strictly below $3.30\text{V}$** with correct polarity.
3. Verify continuity between all ground points (`B-`, STM32 `GND`, ESP32 `GND`, Buck `GND`) before applying power.
4. Do not rely on STM32 firmware alone for primary lithium-ion safety protection.
