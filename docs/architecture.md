# System Architecture & Technical Specification

## 1. Overview

The **Smart Agriculture IoT System** is a distributed telemetry and automated actuation platform designed for greenhouse and high-tech agricultural management. The system consists of:
1. **Wireless Remote Sensor Nodes (ATmega8/ATmega88 + NRF24L01)**: Ultra-low power, battery-operated field units running on internal 8MHz RC oscillator. Each node can host up to 6 capacitive soil moisture probes (P1–P6) to monitor volumetric soil moisture across distinct garden beds without messy field wiring.
2. **Central IoT Gateway / Controller (ESP32-WROOM-32)**: Single-board industrial-grade controller integrating an LM2596 switching buck converter, AMS1117-3.3 linear regulator, ULN2003APG Darlington driver array, and 6 onboard Omron G5LE-14 SPDT relays. It aggregates local microclimate data (DHT11, MQ-135, BH1750, pH probe, water float switch), receives RF telemetry over 2.4 GHz RF24Network, drives an I2C LCD with a 4-button menu interface, and executes automated hysteresis control.
3. **Cloud & Mobile Dashboard (Blynk IoT / WiFiManager)**: Remote telemetry visualization, manual override triggers, and WiFi AP captive portal provisioning.

---

## 2. System Architecture Diagram

```mermaid
graph TD
    subgraph Remote_Edge_Nodes ["Wireless Sensor Nodes (18650 Li-ion + TP4056 + Internal 8MHz)"]
        N1["Node 01: ATmega8 + NRF24L01<br/>Capacitive Soil Probes (A0 - A5)"]
        N2["Node 02: ATmega8 + NRF24L01<br/>Capacitive Soil Probes (A0 - A5)"]
        Nx["Node 0N: NRF24L01 Multi-node Mesh/Star"]
    end

    subgraph Central_Gateway ["Central IoT Gateway (ESP32-WROOM-32 Single Board)"]
        NRF_RX["NRF24L01+ RF Receiver (SPI)<br/>Base Address: Node 00, Channel 90"]
        ESP32["ESP32 Core Microcontroller<br/>Dual Core Tensilica Xtensa @ 240MHz"]
        
        subgraph Local_Sensors ["Local Microclimate Sensors"]
            DHT["DHT11: Temp & Humidity (GPIO32)"]
            FLOAT["Float Switch: Water Level Interlock (GPIO33)"]
            MQ["MQ-135: Air Quality via 3.3k/6.8k Divider (GPIO34 ADC)"]
            PH["Analog pH Probe via 3.3k/6.8k Divider (GPIO35 ADC)"]
            BH["BH1750: Digital Lux Sensor (I2C 0x23, GPIO21/22)"]
        end

        subgraph Local_HMI ["Local Human-Machine Interface (HMI)"]
            LCD["LCD 1602 + PCF8574 I2C (0x27, GPIO21/22)"]
            BTNS["Menu Navigation: THOAT (15), TANG (2), GIAM (4), CHON (16), RST (EN)"]
        end

        subgraph Actuators ["Onboard 6-Channel Relays (ULN2003APG Driver)"]
            R1["Relay 1: Water Pump 1 (GPIO13 -> OUT1)"]
            R2["Relay 2: Water Pump 2 (GPIO12 -> OUT2)"]
            R3["Relay 3: Misting / Humidifier (GPIO14 -> OUT3)"]
            R4["Relay 4: Exhaust / Cooling Fan (GPIO27 -> OUT4)"]
            R5["Relay 5: Supplemental Lighting (GPIO26 -> OUT5)"]
            R6["Relay 6: Heating / Nutrient Valve (GPIO25 -> OUT6)"]
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
    FLOAT --> ESP32
    MQ --> ESP32
    PH --> ESP32
    BH --> ESP32

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
    PWR_MAIN["12V DC External Input (DOMINO U6, 3A - 5A)"]
    
    subgraph Gateway_Power ["Gateway Onboard Power Stages"]
        LM2596["LM2596 Buck Converter 12V -> 5V (L1 1mH + C1/C2 + KC1/KC2)"]
        AMS_GATEWAY["KU1: AMS1117-3.3V Linear Regulator"]
        
        PWR_MAIN -->|12V Rail| RELAY_COILS["Omron G5LE-14 Relay Coils (12V)"]
        PWR_MAIN --> LM2596
        LM2596 -->|5V Rail| ESP32_VIN["ESP32 DevKit Vin Pin"]
        LM2596 -->|5V Rail| LCD_PWR["LCD 1602 VCC (5V)"]
        LM2596 -->|5V Rail| MQ_HTR["MQ-135 Heater VCC (5V, ~160mA)"]
        LM2596 -->|5V Rail| PH_PWR["pH-4502C Signal Conditioner (5V)"]
        LM2596 --> AMS_GATEWAY
        AMS_GATEWAY -->|Clean 3.3V Rail| NRF_GATEWAY["NRF24L01 VCC (+KC2 100nF Filter)"]
        AMS_GATEWAY -->|Clean 3.3V Rail| BH1750_PWR["BH1750 Lux Sensor VCC"]
        AMS_GATEWAY -->|Clean 3.3V Rail| BUTTON_PULLUP["Button Pull-up Resistors R15-R19 (10k)"]
    end

    subgraph Node_Power ["Wireless Node Power Rail"]
        BAT["18650 Li-ion Cell (3.7V - 4.2V, 2600mAh)"]
        TP4056["TP4056 Type-C Charger & Protection IC"]
        AMS_NODE["U3: AMS1117-3.3V (C2 1uF In, C1 1uF Out)"]
        
        BAT <--> TP4056
        TP4056 --> AMS_NODE
        AMS_NODE -->|3.3V DC| ATMEGA8["ATmega8 Microcontroller (Internal 8MHz)"]
        AMS_NODE -->|3.3V DC| NRF_NODE["NRF24L01 Module"]
        AMS_NODE -->|3.3V DC| CAP_SOIL["Capacitive Soil Probes P1-P6"]
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
| **Relay 1** (Pump 1) | Soil Moisture Node 01 | Remote NRF24 (A0) | $< 35\%$ | $> 70\%$ | Water float switch cạn bể |
| **Relay 2** (Pump 2) | Soil Moisture Node 02 | Remote NRF24 (A1) | $< 35\%$ | $> 70\%$ | Water float switch cạn bể |
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
  - Payload buffer: `unsigned long data[6]` containing calibrated soil moisture readings across channels A0 to A5.

---

## 6. Sơ Đồ Nguyên Lý & Layout Bản Vẽ Kỹ Thuật (Official Schematics & PCB)

Dưới đây là các bản vẽ nguyên lý và bố trí linh kiện chính thức được trích xuất trực tiếp từ file thiết kế CAD của tác giả (Kỹ sư La Văn Thịnh):

### A. Sơ Đồ Nguyên Lý Trạm Gateway Trung Tâm (ESP32 Gateway Schematic)
Bản vẽ thể hiện trọn vẹn khối vi điều khiển ESP32, khối hạ áp LM2596, khối đệm ULN2003, dàn 6 rơ-le Omron, mạch phân áp bảo vệ ADC và ma trận phím HMI:

![Sơ đồ nguyên lý Gateway Trung Tâm](../hardware/schematics/png/Schematic_Gateway_Server.png)

### B. Bố Trí Mạch In Trạm Gateway (Gateway PCB Silkscreen & Layout)
Bản vẽ in lụa mặt trên (Top Silkscreen) thể hiện vị trí linh kiện, cọc đấu domino và các phím bấm công nghiệp:

![Bố trí linh kiện PCB Gateway](../hardware/pcb/png/PCB_Gateway_Server.png)

### C. Sơ Đồ Nguyên Lý Điểm Đo Cảm Biến Di Động (ATmega8 Sensor Node Schematic)
Bản vẽ điểm đo không dây ngoài ruộng với vi điều khiển ATmega8 chạy dao động nội 8MHz, module NRF24L01+, khối ổn áp pin AMS1117-3.3V và 6 cổng cắm que đo độ ẩm đất P1–P6:

![Sơ đồ nguyên lý Sensor Node ATmega8](../hardware/schematics/png/Schematic_Sensor_Node_ATmega8.png)

---

## 7. Triển Khai Thực Nghiệm Thực Tế (Field Deployment Showcase)

Hệ thống đã được chế tạo hoàn thiện, lắp ráp cơ khí và vận hành thử nghiệm tại khoa Nông - Lâm nghiệp, Trường Cao đẳng Nghề Lạng Sơn:

| Tủ Điều Khiển Gateway Lắp Ráp Hoàn Chỉnh | Mô Hình Nhà Kính Thủy Canh Thông Minh |
| :---: | :---: |
| ![Gateway Hardware Assembled](images/gateway_hardware_assembled.png) | ![Greenhouse Hydroponic Deployment](images/system_field_deployment_greenhouse.png) |

| Node Cảm Biến Không Dây Hoàn Thiện | Mạch In PCB Node Cảm Biến | Điểm Đo Cắm Đất Thực Tế |
| :---: | :---: | :---: |
| ![Sensor Node Hardware](images/sensor_node_hardware.png) | ![Sensor Node PCB](images/sensor_node_pcb.png) | ![Field Soil Sensor](images/field_sensor_deployment.png) |
