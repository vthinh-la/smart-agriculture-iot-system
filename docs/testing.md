# Testing, Calibration & Quality Assurance

This document outlines the testing protocols, calibration procedures, and validation steps required before commissioning the Smart Agriculture IoT System into production.

---

## 1. Bench Cold Checks (Unpowered)

Before applying external power to the system:
1. **Multimeter Continuity Check**:
   - Verify that there is **NO short-circuit** between:
     - 12V rail and GND ($R > 10\text{k}\Omega$).
     - 5V rail and GND ($R > 1\text{k}\Omega$).
     - 3.3V rail and GND ($R > 500\Omega$).
2. **Polarity Check**:
   - Double-check polarity of all electrolytic capacitors.
   - Verify orientation of IC sockets (notch aligned with PCB silkscreen).
   - Ensure the NRF24 module is not plugged in backwards (Pin 1 GND must face ground pad).

---

## 2. Power Rail Commissioning

1. Apply 12V DC from an external lab bench supply (set current limit to 500mA initially).
2. Measure test points with a DMM:
   - Output of LM2596 buck converter: Must be **$5.00\text{V} \pm 0.15\text{V}$**.
   - Output of AMS1117-3.3V regulator: Must be **$3.30\text{V} \pm 0.05\text{V}$**.
   - Verify 3.3V at Pin 2 of the NRF24 socket.
3. Power down, insert the ESP32 and ATmega8 microcontrollers, and power up again.

---

## 3. Sensor Calibration Procedures

### A. Capacitive Soil Moisture Sensor (Air/Water Calibration)
Capacitive soil sensors output an analog voltage inversely proportional to moisture content. Due to component tolerances on the 555-timer IC, each probe must be calibrated individually:

1. **Dry Air Reference ($0\%$ Moisture)**:
   - Hold the clean probe dry in ambient air.
   - Record analog reading in Serial Monitor: $\text{Raw}_{\text{dry}} \approx 820 - 950$ (on 10-bit ADC).
2. **Water Reference ($100\%$ Moisture)**:
   - Submerge the probe in water up to the white guideline (do not submerge electronics).
   - Record analog reading: $\text{Raw}_{\text{wet}} \approx 380 - 450$.
3. **Firmware Mapping**:
   Update the linear interpolation function in `node01_soil_moisture.ino`:
   ```cpp
   float doam = map(potValue, Raw_dry, Raw_wet, 0, 100);
   doam = constrain(doam, 0, 100);
   ```

---

### B. MQ-135 Hazardous Gas Sensor (Clean Air $R_0$ Baseline)
1. The MQ-135 requires a **24-hour preheat burn-in period** on 5V supply before stable resistance is achieved.
2. Place the sensor in clean outdoor air (away from vehicle exhaust and smoke).
3. Record the sensor analog output voltage $V_{\text{out}}$ and calculate $R_0$:
   $$R_S = \frac{V_{CC} - V_{\text{out}}}{V_{\text{out}}} \times R_L$$
   $$R_0 = \frac{R_S}{3.6} \quad (\text{for clean air})$$
4. Set $R_0$ in `FarmConfig.h` or `main.cpp`.

---

### C. Analog pH Electrode (2-Point Buffer Calibration)
1. Clean the glass bulb of the pH electrode with distilled water and blot dry with lint-free paper.
2. Submerge in standard buffer solution **pH 6.86** at $25^\circ\text{C}$:
   - Adjust the offset potentiometer on the pH-4502C board until output voltage reads **$2.50\text{V}$** (or until serial output shows pH 6.86).
3. Submerge in standard buffer solution **pH 4.01**:
   - Verify voltage drop corresponds to the expected Nernst sensitivity ($\approx 59.16\text{mV} / \text{pH}$).

---

## 4. RF24Network Wireless Link Verification

To verify packet reception and link budget:
1. Power up the Central Gateway (ESP32) and open Serial Monitor at **115200 baud**.
2. Power up Node 01. The onboard status LED should blink every 3 seconds upon packet transmission.
3. Observe Gateway console output:
   ```text
   Received packet from Node 01: Soil Moisture = 48.5%
   ```
4. If packets are dropped or module fails to initialize:
   - Check if `radio.begin()` returns true.
   - Confirm a 10uF - 47uF capacitor is present across NRF24 VCC/GND pins.
   - Adjust `radio.setDataRate(RF24_250KBPS)` for extended range penetration.

---

## 5. Relay Actuation & Hysteresis Verification

1. Simulate low moisture by removing probe from water:
   - Once reading drops below `nguongbat` ($35\%$), verify Relay 1 clicks ON and LED status lights up.
2. Insert probe into water:
   - Observe reading rising past $35\%$; Relay 1 must remain ON.
   - Once reading exceeds `nguongtat` ($70\%$), verify Relay 1 turns OFF cleanly without oscillation.
3. Repeat validation for temperature, humidity, and lighting thresholds.
