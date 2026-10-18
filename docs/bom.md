# Bill of Materials (BOM) & Component Specifications

The component inventory below has been directly verified against project procurement invoices and hardware implementation files.

## 1. Verified Bill of Materials

| No. | Component Name | Model / Specification | Package / Form Factor | Operating Voltage | Qty | Unit Price (VND) | Total (VND) | Primary Function |
| :---: | :--- | :--- | :--- | :---: | :---: | :---: | :---: | :--- |
| **1** | ESP32 Development Board | ESP32-WROOM-32 (NodeMCU / DevKit v1 30-Pin) | DIP-30 Breakout | 5.0V (Vin) / 3.3V | 1 | 93,600 | 93,600 | Central Gateway MCU, WiFi/Blynk engine |
| **2** | AVR Microcontroller | ATmega8A-PU / ATmega328P | DIP-28 | 3.3V - 5.0V | 5 | 37,088 | 185,440 | Remote field sensor node processing |
| **3** | Wireless RF Transceiver | NRF24L01+ 2.4GHz Antenna Module | 2x4 Pin Header (2.54mm) | 1.9V - 3.6V (3.3V nom) | 6 | 13,710 | 82,260 | Wireless sensor telemetry (Mesh/Star) |
| **4** | Capacitive Soil Moisture Sensor | V1.2 / V2.0 Corrosion Resistant | Analog Output PCB Probe | 3.3V - 5.0V | 5 | 11,666 | 58,330 | Soil moisture measurement |
| **5** | Temperature & Humidity Sensor | DHT11 Digital Sensor Module | 3-Pin / 4-Pin Module | 3.3V - 5.0V | 1 | 25,410 | 25,410 | Ambient microclimate monitoring |
| **6** | Air Quality / Gas Sensor | MQ-135 Gas Sensor Module | 4-Pin Breakout (AO/DO) | 5.0V (Heater ~160mA) | 1 | 28,000 | 28,000 | Hazardous gas (NH3, NOx, CO2) detection |
| **7** | Digital Light Sensor | GY-302 (BH1750FVI I2C) | 5-Pin Breakout | 3.0V - 5.0V | 1 | 21,620 | 21,620 | Ambient sunlight lux measurement |
| **8** | Analog pH Sensor Kit | pH-4502C Signal Board + BNC Glass Electrode | PCB Module + BNC Probe | 5.0V DC | 1 | 249,805 | 249,805 | Hydroponic/irrigation water pH analysis |
| **9** | Secondary pH Probe / Sensor | Spare Analog pH conditioning board | PCB Module | 5.0V DC | 1 | 145,150 | 145,150 | Redundant pH channel verification |
| **10** | LCD Display + I2C Backpack | LCD1602 (Blue/Yellow) + PCF8574T I2C Module | 16x2 Character Display | 5.0V DC | 1 | 15,468 | 15,468 | Local Human-Machine Interface (HMI) |
| **11** | Rotary Encoder | EC11 with Integrated Push Switch | 5-Pin Through-Hole | 3.3V / 5.0V | 1 | 14,338 | 14,338 | Menu scrolling and parameter tuning |
| **12** | I/O Expander Module | PCF8575 I2C 16-Bit GPIO Expander | Module Breakout | 2.5V - 5.5V | 1 | 36,572 | 36,572 | Extension for additional relay banks |
| **13** | Voltage Regulator IC | AMS1117-3.3V Low-Dropout Linear Regulator | SOT-223 | Vin up to 12V, Vout 3.3V | 50 (kit) | 28,138 | 28,138 | Clean 3.3V rail for NRF24 and sensors |
| **14** | Li-ion Battery Charger Module | TP4056 Type-C with Protection (DW01A + 8205A) | SMD Board with Type-C | 5.0V In, 4.2V Cut-off | 5 | 5,899 | 29,495 | Wireless node battery management |
| **15** | Battery Enclosure | 1S 18650 Battery Holder with Leads | Single cell slot | 3.7V - 4.2V | 5 | 5,990 | 29,950 | Node autonomous power source |
| **16** | Toggle / Slide Switches | Miniature Slide Switches (Set of 10) | Through-Hole SPDT | 50V 0.5A | 10 (set) | 7,803 | 7,803 | Power ON/OFF switches for nodes |
| **17** | Relay Module (Actuation) | 6-Channel 5V/12V Relay Optocoupled Board | Screw Terminal Block | 5V/12V VCC, 3.3V Logic | 1 | Pre-owned | — | Pumps, fans, lighting, valves control |
| **18** | Custom Enclosures & Mounts | 3D Printed Gateway Case, Sensor Brackets | PLA / PETG FDM | Mechanical | 1 set | DIY Print | — | IP-rated dust and splash enclosure |

---

## 2. Critical Passive & Supporting Components

| Component | Rating / Value | Package | Purpose |
| :--- | :--- | :--- | :--- |
| **Electrolytic / Tantalum Capacitor** | $10\mu\text{F} - 100\mu\text{F}, 16\text{V}$ | Radial / SMD | Direct soldering across NRF24 VCC/GND pins to suppress RF transmission brownout spikes |
| **Ceramic Decoupling Capacitors** | $100\text{nF} (0.1\mu\text{F}), 50\text{V}$ | 0805 / Radial | Placed near ATmega8 VCC/GND and ESP32 power pins |
| **Crystal Resonator** | $16\text{MHz}$ or $8\text{MHz} \pm 20\text{ppm}$ | HC-49S | External clock source for ATmega8 (or use 8MHz internal RC) |
| **Pull-up Resistors** | $4.7\text{k}\Omega - 10\text{k}\Omega, 1/4\text{W}$ | Axial / 0805 | I2C pull-ups on SDA/SCL and DHT11 data line |
| **Tactile Buttons** | $6\times 6\times 5\text{mm}$ 4-Pin Tact Switch | Through-Hole | Menu Select, Back, Up, Down user controls |
