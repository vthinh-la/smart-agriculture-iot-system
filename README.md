# Smart Agriculture IoT Monitoring & Automation System
### Hệ Thống IoT Giám Sát Môi Trường & Điều Khiển Nông Nghiệp Thông Minh

[![Hardware](https://img.shields.io/badge/Hardware-EasyEDA%20%7C%20Sprint--Layout-blue.svg)](file:///hardware/)
[![Firmware](https://img.shields.io/badge/Firmware-PlatformIO%20%7C%20ESP32%20%7C%20AVR-green.svg)](file:///firmware/)
[![3D Print](https://img.shields.io/badge/Mechanical-3D%20STL%20%7C%203MF-orange.svg)](file:///mechanical/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](file:///LICENSE)

---

## 1. Giới thiệu dự án (Project Overview)

**Smart Agriculture IoT System** là hệ thống phần cứng và firmware IoT mã nguồn mở phục vụ nông nghiệp công nghệ cao và nhà màng thông minh. Dự án giải quyết bài toán giám sát đa thông số vi khí hậu và tự động hóa tưới tiêu phân bón chính xác, không dây và tiết kiệm chi phí:

- **Gateway Trung Tâm (ESP32-WROOM-32)**: Thu thập dữ liệu cảm biến cục bộ (Nhiệt độ/Độ ẩm DHT11, Khí độc/Chất lượng không khí MQ-135, Cường độ sáng BH1750, Nồng độ pH nước tưới), đồng thời đóng vai trò Base Station thu nhận sóng 2.4 GHz NRF24L01 từ các node cắm đất từ xa.
- **Node Cảm Biến Không Dây (ATmega8 / ATmega328P + NRF24L01)**: Hoạt động bằng pin Li-ion 18650 tích hợp sạc Type-C TP4056, đo độ ẩm đất điện dung (Capacitive Soil Moisture) chống ăn mòn và truyền gói tin RF định kỳ về Gateway.
- **Hệ Thống Chấp Hành 6 Kênh (6-Relay Bank)**: Điều khiển tự động bơm tưới 1 & 2, máy tạo ẩm phun sương, quạt thông gió, đèn sưởi/quang hợp, van châm dinh dưỡng theo thuật toán chống rung chu kỳ (Hysteresis control).
- **Giao Diện HMI Cục Bộ & Đám Mây**: Màn hình LCD 1602 I2C + phím bấm điều hướng / Encoder EC11 để cài đặt ngưỡng trực tiếp, đồng thời đồng bộ trạng thái lên ứng dụng di động Blynk IoT qua WiFi (tích hợp WiFiManager cấu hình AP).

---

## 2. Kiến trúc hệ thống (System Architecture)

```mermaid
graph TD
    subgraph Nodes ["Wireless Sensor Nodes (Field Units)"]
        N1["Node 01: ATmega8 + NRF24L01<br/>Capacitive Soil Probe 1"]
        N2["Node 02: ATmega8 + NRF24L01<br/>Capacitive Soil Probe 2"]
    end

    subgraph Gateway ["Central Gateway (ESP32-WROOM-32)"]
        NRF["NRF24L01 Receiver (SPI, Ch 90)"]
        MCU["ESP32 Core Processor"]
        
        S1["DHT11 (Temp & Humidity)"]
        S2["MQ-135 (Air Quality / CO2)"]
        S3["BH1750 (Ambient Lux)"]
        S4["Analog pH Sensor Probe"]
        
        LCD["LCD 1602 I2C (0x27)"]
        KEYS["Menu Keys: Select, Back, Up, Down"]
        RELAYS["6-Channel Relays: Pumps, Fans, Lights, Mist"]
    end

    subgraph Connectivity ["Cloud & Network"]
        WIFI["WiFiManager AP Provisioning"]
        BLYNK["Blynk IoT Cloud Dashboard"]
    end

    N1 -->|2.4GHz RF24Network| NRF
    N2 -->|2.4GHz RF24Network| NRF
    NRF --> MCU
    S1 --> MCU
    S2 --> MCU
    S3 --> MCU
    S4 --> MCU
    MCU --> LCD
    KEYS --> MCU
    MCU --> RELAYS
    MCU <--> WIFI
    MCU <--> BLYNK
```

---

## 3. Cấu trúc thư mục mã nguồn & phần cứng

Repository được chuẩn hóa theo cấu trúc mã nguồn mở chuyên nghiệp:

```text
.
├── README.md                           # Tài liệu tổng quan dự án
├── LICENSE                             # Giấy phép mã nguồn mở MIT
├── .gitignore                          # Loại trừ file nhạy cảm, build cache, binaries
├── docs/                               # Tài liệu kỹ thuật chi tiết
│   ├── architecture.md                 # Kiến trúc hệ thống, sơ đồ khối, luồng nguồn
│   ├── bom.md                          # Danh mục linh kiện (BOM) & thông số kỹ thuật
│   ├── wiring.md                       # Sơ đồ đấu nối, bảng pinout và mức điện áp
│   ├── assembly.md                     # Hướng dẫn gia công PCB, hàn & lắp ráp 3D
│   ├── testing.md                      # Quy trình kiểm thử, hiệu chuẩn cảm biến
│   └── known-issues.md                 # Lưu ý thiết kế phần cứng, chống sụt áp RF
├── hardware/                           # Thiết kế mạch điện tử (EDA & Gerber)
│   ├── schematics/
│   │   ├── easyeda/                    # File nguồn nguyên lý EasyEDA JSON
│   │   └── pdf/                        # Sơ đồ nguyên lý ATmega8 PDF
│   ├── pcb/
│   │   ├── easyeda/                    # File nguồn mạch in PCB EasyEDA JSON
│   │   ├── sprint_layout/              # File layout Sprint-Layout 6.0 (.lay6)
│   │   ├── gerber/                     # File Gerber RS-274X & Drill sẵn sàng đặt mạch
│   │   └── etch_masks_pdf/             # Bản in layout 1:1 làm mạch in thủ công
│   └── pinouts/                        # Tài liệu pinout module ESP32, NRF24, MQ135
├── mechanical/                         # Thiết kế cơ khí & vỏ hộp in 3D
│   └── 3d_models/
│       ├── gateway_enclosure/          # Vỏ hộp Gateway Box & Lid (STL, 3MF)
│       ├── sensor_brackets/            # Đế gá cảm biến BH1750, chụp bảo vệ MQ-135
│       └── node_enclosure/             # Vỏ hộp node không dây RF-Nano (STL)
└── firmware/                           # Mã nguồn lập trình nhúng
    ├── gateway_esp32/                  # Dự án PlatformIO hoàn chỉnh cho ESP32
    │   ├── platformio.ini
    │   ├── src/main.cpp
    │   ├── include/FarmConfig.h
    │   └── lib/                        # Thư viện driver nRF24, Blynk, MQ135...
    ├── gateway_arduino_ide/            # Phiên bản sketch Arduino IDE dự phòng
    └── sensor_nodes_atmega/            # Firmware cho các Node ATmega8 / 328P
        ├── node01_soil_moisture/
        ├── node02_soil_moisture/
        └── receiver_test/
```

---

## 4. Danh mục linh kiện chính (BOM Summary)

| Linh kiện | Model / Thông số | Vai trò | Số lượng |
| :--- | :--- | :--- | :---: |
| **ESP32 DevKit** | ESP32-WROOM-32 (30-pin) | Gateway trung tâm, WiFi/Blynk, HMI | 1 |
| **Vi điều khiển AVR** | ATmega8A-PU / ATmega328P DIP-28 | MCU đọc cảm biến cho các Node ngoài vườn | 5 |
| **Module RF 2.4GHz** | NRF24L01+ | Truyền nhận không dây cự ly xa (Mesh/Star) | 6 |
| **Cảm biến độ ẩm đất** | Capacitive Soil Moisture v1.2/v2.0 | Cảm biến điện dung chống ăn mòn | 5 |
| **Cảm biến nhiệt ẩm** | DHT11 Digital Sensor | Đo nhiệt độ và độ ẩm không khí | 1 |
| **Cảm biến khí gas** | MQ-135 Air Quality Sensor | Giám sát khí NH3, CO2, mùi hữu cơ | 1 |
| **Cảm biến ánh sáng** | GY-302 (BH1750 Digital Lux I2C) | Đo quang thông ánh sáng mặt trời | 1 |
| **Cảm biến đo độ pH** | pH-4502C + Đầu dò BNC Glass Probe | Đo độ pH dung dịch dinh dưỡng/nước tưới | 1 |
| **Màn hình hiển thị** | LCD1602 + PCF8574 I2C Backpack | Hiển thị thông số tại tủ điều khiển | 1 |
| **Bộ 6 Relay** | Module Relay 6 kênh cách ly quang | Đóng cắt bơm, quạt, đèn, van điện từ | 1 |
| **Bộ sạc & Pin** | TP4056 Type-C + Cell Li-ion 18650 | Nguồn độc lập cho các Node ngoài vườn | 5 |

*Xem chi tiết toàn bộ mã linh kiện, đơn giá và quy cách tại [docs/bom.md](file:///docs/bom.md).*

---

## 5. Hướng dẫn nạp Firmware (Getting Started)

### Yêu cầu môi trường
- [Visual Studio Code](https://code.visualstudio.com/) + Extension [PlatformIO IDE](https://platformio.org/)
- Hoặc [Arduino IDE 2.x](https://www.arduino.cc/en/software) với ESP32 Board Package và MiniCore (cho ATmega8)

### Cách 1: Nạp Gateway ESP32 bằng PlatformIO (Khuyến nghị)
1. Mở thư mục [`firmware/gateway_esp32`](file:///firmware/gateway_esp32) bằng VS Code PlatformIO.
2. Cắm cáp Micro-USB / Type-C kết nối ESP32 với máy tính.
3. Chạy lệnh build & nạp:
   ```bash
   pio run --target upload
   pio device monitor -b 115200
   ```
4. Khi khởi động lần đầu, ESP32 sẽ phát WiFi AP để bạn dùng điện thoại kết nối và cài đặt WiFi nhà kính qua giao diện web.

### Cách 2: Nạp Firmware cho Node ATmega8
1. Mở file [`firmware/sensor_nodes_atmega/node01_soil_moisture/node01_soil_moisture.ino`](file:///firmware/sensor_nodes_atmega/node01_soil_moisture/node01_soil_moisture.ino) trong Arduino IDE.
2. Chọn Board: **MiniCore -> ATmega8**.
   - Clock: **Internal 8 MHz** hoặc **External 16 MHz** tùy mạch thực tế.
   - Bootloader: Yes / No.
3. Sử dụng mạch nạp USBasp hoặc Arduino as ISP để nạp code cho chip.

---

## 6. Trạng thái kiểm thử phần cứng & Xác minh thực tế

- [x] **Mạch in & Hàn linh kiện**: Đã hoàn thành chế tạo PCB 1 lớp và hàn hoàn thiện mạch Gateway và Node.
- [x] **Truyền nhận NRF24**: Đã kiểm thử giao tiếp ổn định giữa Node 01, Node 02 và Gateway ở tốc độ 2Mbps Channel 90 (đã gắn tụ $10\mu\text{F}$ chống sụt áp RF).
- [x] **Đọc cảm biến**: Cảm biến DHT11, BH1750, MQ-135 và cảm biến điện dung đất đọc chính xác giá trị thực.
- [x] **Màn hình & Phím bấm**: Hệ thống menu LCD 1602 điều khiển bằng 4 phím bấm hoạt động mượt mà.
- [x] **Rơ-le tự động**: Thuật toán Hysteresis đóng ngắt bơm và quạt đúng ngưỡng cài đặt.
- [x] **Vỏ hộp in 3D**: Vỏ hộp Gateway Box và đế cảm biến BH1750 khớp cơ khí và bảo vệ tốt bo mạch.

---

## 7. Giấy phép & Đóng góp (License)

Dự án được phát hành theo giấy phép mã nguồn mở **[MIT License](file:///LICENSE)**. Mọi cá nhân, sinh viên, kỹ sư và doanh nghiệp tự do sử dụng, chỉnh sửa, thương mại hóa và phát triển tiếp.
