# Connections and electrical safety

Add a clear wiring diagram and document:
- Battery cell arrangement and maximum pack voltage.
- Voltage-divider resistor values, divider ratio and ADC input limits.
- Sensor supply voltages and output ranges.
- Ground/reference arrangement between STM32, ESP32 and sensors.
- MOSFET gate-driver arrangement and load supply.
- Fuse, current limiting and emergency disconnect arrangements.

**Safety:** Never connect a cell or pack voltage directly to an MCU ADC pin unless the input is guaranteed to remain within the MCU's absolute maximum ratings. Verify divider design, polarity, grounding and isolation requirements before powering the circuit. Do not rely on firmware alone for battery safety.
