# System Architecture & Technical Specification

## 1. Overview

The **Smart Agriculture IoT System** is a distributed telemetry and automated actuation platform designed for greenhouse and high-tech agricultural management. The system consists of:
1. **Wireless Remote Sensor Nodes (ATmega8/ATmega328P + NRF24L01)**: Battery-powered edge units deployed in the soil bed to monitor volumetric soil moisture continuously without cable clutter.
2. **Central IoT Gateway / Controller (ESP32-WROOM-32)**: Central hub that collects local multi-parameter sensor data (Air Temperature & Humidity, Toxic Gases / Air Quality, Ambient Sunlight Luminosity, Water pH), receives remote soil moisture metrics over 2.4 GHz RF24Network, renders real-time diagnostics on an I2C LCD, and executes intelligent hysteresis-based relay automation for irrigation, ventilation, lighting, and misting.
3. **Cloud & Mobile Dashboard (Blynk IoT / WiFiManager)**: Remote telemetry visualization, manual override triggers, and WiFi AP captive portal provisioning.

---

## 2. System Architecture Diagram

```mermaid
graph TD
    subgraph Remote_Edge_Nodes ["Wireless Sensor Nodes (Battery 18650 + TP4056)"]
        N1["Node 01: ATmega8 + NRF24L01<br/>Capacitive Soil Moisture 1"]
        N2["Node 02: ATmega8 + NRF24L01<br/>Capacitive Soil Moisture 2"]
        Nx["Node 0N: NRF24L01 Mesh Extensible"]
    end

    subgraph Central_Gateway ["Central IoT Gateway (ESP32-WROOM-32)"]
        NRF_RX["NRF24L01+ RF Receiver (SPI)<br/>Base Address: Node 00, Channel 90"]
        ESP32["ESP32 Core Microcontroller<br/>Dual Core Tensilica Xtensa @ 240MHz"]
        
        subgraph Local_Sensors ["Local Microclimate Sensors"]
            DHT["DHT11: Temp & Humidity (GPIO32)"]
            MQ["MQ-135: Air Quality / NH3 / CO2 (GPIO34 ADC)"]
            BH["BH1750: Digital Lux Sensor (I2C 0x23)"]
            PH["Analog pH Sensor Probe (GPIO35 ADC)"]
        end

        subgraph Local_HMI ["Local Human-Machine Interface (HMI)"]
            LCD["LCD 1602 + PCF8574 I2C (0x27)"]
            BTNS["Menu Navigation: Select, Back, Up, Down / EC11"]
        end

        subgraph Actuators ["6-Channel Optocoupled Relay Bank"]
            R1["Relay 1: Water Pump (GPIO13)"]
            R2["Relay 2: Secondary Irrigation (GPIO12)"]
            R3["Relay 3: Misting / Humidifier (GPIO14)"]
            R4["Relay 4: Exhaust / Cooling Fan (GPIO27)"]
            R5["Relay 5: Supplemental Lighting (GPIO26)"]
            R6["Relay 6: Heating / Nutrient Valve (GPIO25)"]
        end
    end

    subgraph Cloud_Mobile ["Cloud & Network Connectivity"]
        WIFI_AP["WiFiManager: On-demand Captive Portal AP"]
        BLYNK["Blynk IoT Cloud / Private Server"]
        APP["Blynk Mobile Application & Web Dashboard"]
    end

    %% RF Connections
    N1 -->|2.4GHz RF24Network Packet| NRF_RX
    N2 -->|2.4GHz RF24Network Packet| NRF_RX
    Nx -->|2.4GHz RF24Network Packet| NRF_RX
    NRF_RX --> ESP32

    %% Local Sensor Connections
    DHT --> ESP32
    MQ --> ESP32
    BH --> ESP32
    PH --> ESP32

    %% Actuation & HMI
    ESP32 --> LCD
    BTNS --> ESP32
    ESP32 --> R1
    ESP32 --> R2
    ESP32 --> R3
    ESP32 --> R4
    ESP32 --> R5
    ESP32 --> R6

    %% Network
    ESP32 <--> WIFI_AP
    ESP32 <-->|WiFi 802.11 b/g/n| BLYNK
    BLYNK <--> APP
```

---

## 3. Power Distribution Tree

```mermaid
graph LR
    PWR_MAIN["12V DC External Power Supply (3A - 5A)"]
    
    subgraph Gateway_Power ["Gateway Power Rail"]
        LM2596["Buck Converter 12V -> 5V DC (3A)"]
        AMS_3V3["LDO AMS1117-3.3V"]
        
        PWR_MAIN -->|12V Rail| RELAY_COILS["Relay Board Solenoid Coils (12V)"]
        PWR_MAIN --> LM2596
        LM2596 -->|5V Rail| ESP32_VIN["ESP32 DevKit 5V Pin"]
        LM2596 -->|5V Rail| LCD_PWR["LCD 1602 VCC (5V)"]
        LM2596 -->|5V Rail| MQ_HTR["MQ-135 Heater VCC (5V, ~160mA)"]
        LM2596 -->|5V Rail| PH_PWR["pH Signal Conditioner VCC (5V)"]
        LM2596 --> AMS_3V3
        AMS_3V3 -->|Dedicated Clean 3.3V| NRF_GATEWAY["NRF24L01 VCC (+100uF Tantalum/Electrolytic Cap)"]
        AMS_3V3 -->|3.3V Rail| BH1750_PWR["BH1750 Light Sensor VCC"]
        AMS_3V3 -->|3.3V Rail| DHT_PWR["DHT11 Sensor VCC"]
    end

    subgraph Node_Power ["Wireless Node Power Rail"]
        BAT["18650 Li-ion Cell (3.7V - 4.2V, 2600mAh)"]
        TP4056["TP4056 Type-C Charger & Protection IC"]
        AMS_NODE["AMS1117-3.3V Ultra-low Ripple LDO"]
        
        BAT <--> TP4056
        TP4056 --> AMS_NODE
        AMS_NODE -->|3.3V DC| ATMEGA8["ATmega8 Microcontroller"]
        AMS_NODE -->|3.3V DC| NRF_NODE["NRF24L01 Module (+10uF Cap)"]
        AMS_NODE -->|3.3V DC| CAP_SOIL["Capacitive Soil Moisture Probe (v1.2)"]
    end
```

---

## 4. Control Logic & Hysteresis Algorithm

To prevent rapid oscillation (relay chattering / hunting) near trigger setpoints, all actuator channels implement dual-threshold hysteresis:

$$\text{Actuator State} = \begin{cases} 
\text{ON} & \text{if } \text{SensorValue} < \text{Threshold}_{\text{ON}} \\
\text{OFF} & \text{if } \text{SensorValue} > \text{Threshold}_{\text{OFF}} \\
\text{Unchanged} & \text{if } \text{Threshold}_{\text{ON}} \le \text{SensorValue} \le \text{Threshold}_{\text{OFF}}
\end{cases}$$

### Actuation Matrix:
| Actuator | Control Parameter | Sensor Source | Default ON Trigger | Default OFF Trigger | Safety Interlock |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Relay 1** (Pump 1) | Soil Moisture Node 01 | Remote NRF24 | $< 35\%$ | $> 70\%$ | Max continuous run timer |
| **Relay 2** (Pump 2) | Soil Moisture Node 02 | Remote NRF24 | $< 35\%$ | $> 70\%$ | Max continuous run timer |
| **Relay 3** (Humidifier) | Air Humidity | DHT11 (Local) | $< 50\%$ | $> 75\%$ | Anti-dry run protection |
| **Relay 4** (Exhaust Fan) | Air Temp / Gas PPM | DHT11 / MQ-135 | $> 32^\circ\text{C}$ or $> 300\text{ ppm}$ | $< 27^\circ\text{C}$ and $< 150\text{ ppm}$ | Manual override |
| **Relay 5** (Lighting) | Sunlight Intensity | BH1750 (Local) | $< 500\text{ Lux}$ | $> 2500\text{ Lux}$ | Schedule & Lux combo |
| **Relay 6** (Nutrient/Aux) | Water pH level | Analog pH Probe | Outside $[5.8, 6.8]$ | Inside $[6.0, 6.5]$ | Dose rate limiter |

---

## 5. RF24Network Communication Protocol

- **Frequency Band**: 2.400 GHz - 2.4835 GHz ISM Band.
- **RF Channel**: Channel 90 ($2400 + 90 = 2490\text{ MHz}$) to isolate from standard 2.4GHz WiFi channels (Channels 1, 6, 11).
- **Data Rate**: `RF24_2MBPS` / `RF24_1MBPS`.
- **Addressing**:
  - Central Gateway: Base Node `00` (Octal).
  - Field Sensor 1: Node `01` (Octal).
  - Field Sensor 2: Node `02` (Octal).
- **Packet Structure**:
  - `RF24NetworkHeader(node00)`
  - Payload buffer: `float data[6]` containing calibrated sensor floats, battery status, and node ID.

---

## 6. Hardware Implementation & Physical Layout Gallery

### A. Central Gateway Controller
The Gateway integrates the ESP32 compute module, power buck regulation, relay isolation, LCD I2C backpack, and the NRF24 RF frontend.

| Assembled Hardware | 3D Board Visualization |
| :---: | :---: |
| ![Gateway Hardware Assembled](images/gateway_hardware_assembled.png) | ![Gateway 3D PCB Render](images/gateway_3d_render.png) |

| PCB Layout - Top Copper / Silkscreen | PCB Layout - Bottom Copper |
| :---: | :---: |
| ![Gateway PCB Top](images/gateway_pcb_top.png) | ![Gateway PCB Bottom](images/gateway_pcb_bottom.png) |

---

### B. Remote Sensor Nodes (Points of Measurement)
Each remote node comprises an ATmega8 MCU, NRF24L01 radio, capacitive soil moisture probe, 18650 Li-ion battery, and TP4056 charging circuit.

| Assembled Sensor Node | Sensor Node PCB Layout | Field Deployment in Soil Bed |
| :---: | :---: | :---: |
| ![Sensor Node Hardware](images/sensor_node_hardware.png) | ![Sensor Node PCB](images/sensor_node_pcb.png) | ![Field Soil Sensor](images/field_sensor_deployment.png) |

---

### C. Greenhouse Pilot Deployment
The complete system was deployed and evaluated on a pilot hydroponic and automated greenhouse installation at the Agriculture Department of Lang Son Vocational College:

![Greenhouse Hydroponic Deployment](images/system_field_deployment_greenhouse.png)
