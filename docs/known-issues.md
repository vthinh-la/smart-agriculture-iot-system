# Known Issues, Hardware Quirks & Design Constraints

This document catalogs known hardware behaviors, electronic edge cases, and design constraints identified during the design and fabrication of the Smart Agriculture IoT System.

---

## 1. NRF24L01+ Power Transient Brownouts
- **Symptom**: Transceiver fails to initialize (`radio.begin() == false`) or drops packets intermittently under load.
- **Root Cause**: During transmission bursts, the NRF24 draws instantaneous peak currents up to 115mA (with PA+LNA). Linear regulators with slow transient response allow the 3.3V rail to dip below the 1.9V minimum threshold, triggering an internal brown-out reset.
- **Hardware Fix**:
  - The Gateway board includes a dedicated **AMS1117-3.3V** regulator ($KU_1$) with capacitor $KC_2$ powering the NRF24 module independently from MCU logic.
  - On the Node board, filter capacitor $C_1$ ($1\mu\text{F} - 10\mu\text{F}$) is positioned directly adjacent to the AMS1117-3.3V output and transceiver power pins.

---

## 2. ESP32 ADC Non-Linearity & 5V Level Scaling
- **Symptom**: Analog readings from the pH probe (GPIO35) and MQ-135 (GPIO34) show non-linear distortion at the lower ($< 0.15\text{V}$) and upper ($> 3.15\text{V}$) voltage boundaries.
- **Root Cause**: The SAR ADC inside the ESP32 suffers from inherent non-linearity across its 12-bit range ($0 - 4095$).
- **Hardware Protection**: Both MQ-135 ($P_1$) and pH-4502C ($U_7$) run from the +5V rail. To prevent exceeding the ESP32 3.3V absolute maximum rating, the Gateway board incorporates dual onboard voltage dividers:
  - MQ-135: $R_9 (3.3\text{k}\Omega) + R_{11} (6.8\text{k}\Omega) \rightarrow \text{GPIO34}$
  - pH-4502C: $R_{10} (3.3\text{k}\Omega) + R_{12} (6.8\text{k}\Omega) \rightarrow \text{GPIO35}$
  - Attenuation factor: $\frac{6.8}{3.3 + 6.8} = 0.673$, scaling $5.0\text{V} \rightarrow 3.37\text{V}$ max.
- **Software Mitigation**: Multi-sample rolling averaging (16 samples per reading) is applied in firmware.

---

## 3. ESP32 Boot Strapping Pins & Button Matrix
- **Symptom**: ESP32 fails to boot into flash execution mode or enters download bootloader unexpectedly.
- **Root Cause**: Certain pins are used as strapping pins during power-up:
  - **GPIO 12 (MTDI)**: Must be LOW during boot for 3.3V SPI flash operation. Connected to Relay 2 via ULN2003 (ULN2003 inputs present a high-impedance pull-down to GND through internal base resistors, keeping GPIO12 LOW at boot).
  - **GPIO 15 (MTDO)**: Controls ROM bootloader debug output. Connected to Back/Exit Button (`THOAT` / `nut_thoat`) with $10\text{k}\Omega$ pull-up ($R_{15}$).
  - **GPIO 2**: Must be floating or LOW to enter bootloader. Connected to Increment/Up Button (`TANG` / `nut_tang`) with $10\text{k}\Omega$ pull-up ($R_{17}$).
  - **GPIO 4**: Connected to Decrement/Down Button (`GIAM` / `nut_giam`) with $10\text{k}\Omega$ pull-up ($R_{16}$).
  - **GPIO 16**: Connected to Select Button (`KEY2` / `nut_chon`) with $10\text{k}\Omega$ pull-up ($R_{18}$).
- **Design Rule**: Ensure pull-ups/pull-downs on these lines do not force invalid logic states during the power-on reset window ($< 100\text{ms}$).

---

## 4. MQ-135 Heater Current & Thermal Dissipation
- **Symptom**: 5V rail drops or 5V regulator overheats.
- **Root Cause**: The internal tin dioxide ($SnO_2$) heating element of the MQ-135 requires $5.0\text{V} \pm 0.2\text{V}$ and draws approximately **$150\text{mA} - 180\text{mA}$** continuous current.
- **Design Rule**: The MQ-135 is powered directly from the primary 5V buck converter (LM2596-5.0), never from the ESP32 internal 3.3V regulator or MCU GPIO pins.

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
