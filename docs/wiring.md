# Wiring Guide & Pinout Specifications

This document defines the physical and electrical interconnections for the Central Gateway (ESP32) and the Wireless Sensor Nodes (ATmega8).

---

## 1. Central Gateway (ESP32-WROOM-32) Pin Mapping

### A. Communication Buses (I2C & SPI)

| Peripheral | Signal | ESP32 Pin | Logic Level | Operating VCC | Notes |
| :--- | :--- | :--- | :---: | :---: | :--- |
| **I2C Bus** (LCD1602, BH1750, PCF8575) | SDA | **GPIO 21** | 3.3V | 5V / 3.3V | 4.7kΩ pull-up to 3.3V recommended |
| | SCL | **GPIO 22** | 3.3V | 5V / 3.3V | 4.7kΩ pull-up to 3.3V recommended |
| **NRF24L01+ RF Transceiver** | CE | **GPIO 17** | 3.3V | **3.3V ONLY** | Chip Enable (Transmit/Receive mode) |
| | CSN | **GPIO 5** | 3.3V | **3.3V ONLY** | SPI Chip Select Not |
| | SCK | **GPIO 18** | 3.3V | **3.3V ONLY** | SPI Serial Clock |
| | MISO | **GPIO 19** | 3.3V | **3.3V ONLY** | Master In Slave Out |
| | MOSI | **GPIO 23** | 3.3V | **3.3V ONLY** | Master Out Slave In |
| | IRQ | NC / Unused | — | — | Can be connected to GPIO 16 if interrupt-driven |
| | VCC / GND | 3.3V / GND | — | 3.3V | **Must place 10uF-100uF cap across VCC and GND** |

> [!CAUTION]
> **DO NOT connect NRF24L01+ VCC to 5V!** While the SPI data inputs are 5V-tolerant, the power pin VCC is rated for a maximum of 3.6V. Connecting VCC to 5V will instantly destroy the transceiver chip.

---

### B. Local Sensor Interfaces

| Sensor Module | Output Pin | ESP32 Pin | Type | Power Supply | Electrical Notes |
| :--- | :--- | :--- | :---: | :---: | :--- |
| **DHT11** (Temp & Humidity) | DATA | **GPIO 32** | Digital (1-Wire) | 3.3V or 5V | Requires 10kΩ pull-up to VCC |
| **MQ-135** (Air Quality / Gas) | AOUT | **GPIO 34** | Analog (ADC1_CH6) | **5.0V** | Sensor heater requires 5V ~160mA; use voltage divider if AOUT exceeds 3.3V |
| **Analog pH Sensor Board** | Po (pH Out) | **GPIO 35** | Analog (ADC1_CH7) | **5.0V** | Output voltage corresponds to pH 0-14 ($V_{\text{mid}} \approx 2.5\text{V}$ at pH 7) |
| **BH1750** (Ambient Lux) | I2C | GPIO 21 / 22 | Digital (I2C) | 3.3V | Address: `0x23` (ADDR tied to GND) |

> [!NOTE]
> GPIO 34 and GPIO 35 on ESP32 are **Input-Only** pins and belong to **ADC1**. They do not conflict with the WiFi peripheral (unlike ADC2 pins), allowing stable continuous analog sampling during active WiFi transmission.

---

### C. Actuators (6-Channel Relay Board)

| Channel | Controlled Device | ESP32 Pin | Active Level | Default Safety State | Max Current Rating |
| :---: | :--- | :--- | :---: | :---: | :--- |
| **Relay 1** | Irrigation Pump 1 (Zone 1) | **GPIO 13** | Active HIGH / LOW | OFF | 10A @ 250VAC / 30VDC |
| **Relay 2** | Irrigation Pump 2 (Zone 2) | **GPIO 12** | Active HIGH / LOW | OFF | 10A @ 250VAC / 30VDC |
| **Relay 3** | Misting / Humidifier | **GPIO 14** | Active HIGH / LOW | OFF | 10A @ 250VAC / 30VDC |
| **Relay 4** | Exhaust Fan / Ventilation | **GPIO 27** | Active HIGH / LOW | OFF | 10A @ 250VAC / 30VDC |
| **Relay 5** | Supplemental Grow Lights | **GPIO 26** | Active HIGH / LOW | OFF | 10A @ 250VAC / 30VDC |
| **Relay 6** | Nutrient Dosing Valve / Heater | **GPIO 25** | Active HIGH / LOW | OFF | 10A @ 250VAC / 30VDC |

---

### D. User Interface Controls (Tactile Buttons / Encoder)

| Function | Tactile Button / Pin | ESP32 Pin | Mode | Description |
| :--- | :--- | :--- | :---: | :--- |
| **Select / Enter** | `nut_chon` | **GPIO 15** | Input (Pull-up) | Enter menu, confirm parameter changes |
| **Back / Exit** | `nut_thoat` | **GPIO 16** | Input (Pull-up) | Return to parent menu or home screen |
| **Up / Increment** | `nut_tang` | **GPIO 4** | Input (Pull-up) | Increase threshold value, scroll up |
| **Down / Decrement** | `nut_giam` | **GPIO 2** | Input (Pull-up) | Decrease threshold value, scroll down |

---

## 2. Wireless Field Sensor Node (ATmega8 / ATmega328P) Pin Mapping

| ATmega8 Pin | Physical Pin (DIP-28) | Peripheral Connection | Signal | Description |
| :---: | :---: | :--- | :---: | :--- |
| **PD7** | Pin 13 | NRF24L01+ | **CE** | Chip Enable (Active HIGH transmit/receive) |
| **PB0** | Pin 14 | NRF24L01+ | **CSN** | SPI Chip Select (Active LOW) |
| **PB5** | Pin 19 | NRF24L01+ | **SCK** | Hardware SPI Clock |
| **PB4** | Pin 18 | NRF24L01+ | **MISO** | Hardware SPI Master-In Slave-Out |
| **PB3** | Pin 17 | NRF24L01+ | **MOSI** | Hardware SPI Master-Out Slave-In |
| **PC0 (ADC0)** | Pin 23 | Capacitive Soil Sensor | **AOUT** | Analog soil moisture measurement ($0 - 3.3\text{V}$) |
| **PB1** | Pin 15 | Status Indicator | **LED** | Blinks upon packet transmission / error indication |
| **VCC / AVCC** | Pins 7, 20 | Power Rail | **3.3V DC** | Powered by AMS1117-3.3V from 18650 cell |
| **GND** | Pins 8, 22 | Ground | **GND** | Common system ground |
| **RESET** | Pin 1 | Pull-up Resistor | **RST** | 10kΩ pull-up to 3.3V with 100nF to DTR for programming |

---

## 3. High-Frequency Noise Suppression Circuit (Mach Chong Nhieu)

To isolate inductive kickback and electrical noise from pumps and relay coils:
1. **Flyback Diodes**: 1N4007 diodes installed in reverse parallel across all DC solenoid valves and pump inductive coils.
2. **RC Snubber**: $0.1\mu\text{F} \text{ (X2 rated)} + 100\Omega \text{ (1W)}$ across AC load relay contacts.
3. **Decoupling Capacitors**: Place $100\text{nF}$ ceramic caps as close as possible to the VCC/GND pins of the ESP32 and ATmega8 ICs.
