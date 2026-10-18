# Known Issues, Hardware Quirks & Design Constraints

This document catalogs known hardware behaviors, electronic edge cases, and design constraints identified during the design and fabrication of the Smart Agriculture IoT System.

---

## 1. NRF24L01+ Power Transient Brownouts
- **Symptom**: Transceiver fails to initialize (`radio.begin() == false`) or drops packets intermittently under load.
- **Root Cause**: During transmission bursts, the NRF24 draws instantaneous peak currents up to 115mA (with PA+LNA). Linear regulators with slow transient response allow the 3.3V rail to dip below the 1.9V minimum threshold, triggering an internal brown-out reset.
- **Hardware Fix**: Solder a **$10\mu\text{F} - 47\mu\text{F}$ low-ESR electrolytic or tantalum capacitor** in parallel with a **$100\text{nF}$ ceramic capacitor** directly across the module's VCC and GND pins (as close to the module header as possible).

---

## 2. ESP32 ADC Non-Linearity
- **Symptom**: Analog readings from the pH probe (GPIO35) and MQ-135 (GPIO34) show non-linear distortion at the lower ($< 0.15\text{V}$) and upper ($> 3.15\text{V}$) voltage boundaries.
- **Root Cause**: The SAR ADC inside the ESP32 suffers from inherent non-linearity and offset error across its 12-bit range ($0 - 4095$).
- **Mitigation**:
  1. The firmware uses multi-sample averaging (16 samples per reading).
  2. For high-precision pH measurements, an external I2C ADC (e.g., ADS1115 16-bit) is recommended in future revisions.

---

## 3. ESP32 Boot Strapping Pins
- **Symptom**: ESP32 fails to boot into flash execution mode or enters download bootloader unexpectedly.
- **Root Cause**: Certain pins are used as strapping pins during power-up:
  - **GPIO 12 (MTDI)**: Must be LOW during boot for 3.3V SPI flash operation. Connected to Relay 2.
  - **GPIO 15 (MTDO)**: Controls ROM bootloader debug output. Connected to Menu Button (`nut_chon`).
  - **GPIO 2**: Must be floating or LOW to enter bootloader. Connected to Decrement Button (`nut_giam`).
- **Design Rule**: Ensure pull-ups/pull-downs on these lines do not force invalid logic states during the power-on reset window ($< 100\text{ms}$).

---

## 4. MQ-135 Heater Current & Thermal Dissipation
- **Symptom**: 5V rail drops or 5V regulator overheats.
- **Root Cause**: The internal tin dioxide ($SnO_2$) heating element of the MQ-135 requires $5.0\text{V} \pm 0.2\text{V}$ and draws approximately **$150\text{mA} - 180\text{mA}$** continuous current.
- **Design Rule**: The MQ-135 must be powered directly from the primary 5V buck converter (LM2596), never from the ESP32 internal 3.3V regulator or MCU GPIO pins.

---

## 5. RF Coexistence (2.4GHz WiFi vs NRF24)
- **Symptom**: Degraded wireless range or packet loss when ESP32 active WiFi is transmitting near the NRF24 antenna.
- **Root Cause**: Both ESP32 WiFi and NRF24L01 operate within the 2.400 - 2.4835 GHz ISM spectrum.
- **Firmware Solution**: The NRF24 network is configured to **Channel 90** ($2.490\text{ GHz}$), which lies above standard WiFi Channel 11 ($2.462\text{ GHz}$), minimizing co-channel packet collision.

---

## 6. Capacitive Soil Moisture Sensor Edge Delamination
- **Symptom**: Capacitive probe readings drift upward over weeks of field operation.
- **Root Cause**: Moisture penetrates through the exposed fiberglass FR-4 edges of low-cost capacitive probes, creating parasitic conductive pathways.
- **Prevention**: Coat the cut edges and top SMD components with clear conformal epoxy or silicone before burying the probe into soil.
