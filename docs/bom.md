# Bill of Materials (BOM) & Component Specifications

Bảng kê linh kiện dưới đây được đối soát, chuẩn hóa và khớp nối 100% theo bản vẽ nguyên lý và mạch in chính thức của dự án:
- **Mạch Gateway**: [`Schematic_Gateway_Server.png`](../hardware/schematics/png/Schematic_Gateway_Server.png) và [`PCB_Gateway_Server.png`](../hardware/pcb/png/PCB_Gateway_Server.png)
- **Mạch Sensor Node**: [`Schematic_Sensor_Node_ATmega8.png`](../hardware/schematics/png/Schematic_Sensor_Node_ATmega8.png) và [`PCB_Sensor_Node_ATmega8_Etch_Mask.pdf`](../hardware/pcb/etch_masks_pdf/PCB_Sensor_Node_ATmega8_Etch_Mask.pdf)

---

## 1. Bo Mạch Điều Khiển Trung Tâm (Central Gateway Controller)

Bo mạch Gateway là mạch tích hợp nguyên khối (Custom Single-Board Controller) bao gồm vi xử lý ESP32, bộ nguồn xung hạ áp LM2596-5.0V, ổn áp phụ AMS1117-3.3V, khối đệm công suất Darlington ULN2003APG điều khiển trực tiếp 6 rơ-le Omron, các mạch cầu phân áp bảo vệ ADC 3.3V, cổng cắm màn hình LCD I2C, cổng phao nước và các header cảm biến vi khí hậu.

| Ký hiệu trên sơ đồ | Tên linh kiện | Model / Giá trị / Thông số kỹ thuật | Kiểu đóng vỏ (Package) | Điện áp & Dòng | Số lượng | Vai trò trong hệ thống |
| :---: | :--- | :--- | :---: | :---: | :---: | :--- |
| **U1** | Module vi điều khiển | **ESP32 DevKit V1 (38 Pins)** | DIP-38 Header Cái | 5V Vin / 3.3V Logic | 1 | Bộ xử lý trung tâm, WiFi 802.11 b/g/n, Blynk Cloud, Web OTA |
| **U5** | IC Nguồn xung hạ áp | **LM2596-5.0 / LM2596 ADJ** | TO-263-5 / TO-220 | $V_{\text{in}} = 12\text{V}$, $V_{\text{out}} = 5\text{V}$, 3A | 1 | Hạ áp hiệu suất cao từ nguồn tổng 12V xuống 5V cho ESP32, LCD, MQ135 |
| **L1** | Cuộn cảm lọc nguồn xung | Cuộn cảm dây quấn lõi Ferit (Toroidal) | Through-Hole Chân Cắm | $1\text{mH} / 33-100\mu\text{H}$, dòng chịu tải 3A | 1 | Tích trữ năng lượng và san phẳng dòng ngõ ra cho LM2596 |
| **C1, C2** | Tụ gốm lọc cao tần | Tụ gốm nhiều lớp (MLCC) $0.1\mu\text{F}$ (104) | Radial P=2.5mm | 50V DC | 2 | Lọc nhiễu xung cao tần ngõ vào và ra của LM2596 |
| **KC1** | Tụ hóa nhôm lọc nguồn | Tụ phân cực $10\mu\text{F} / 25\text{V}$ | Radial Chân Cắm | 25V | 1 | Ổn định điện áp DC 5V sau cuộn cảm L1 |
| **KC2 (5V rail)** | Tụ gốm lọc nguồn | Tụ gốm $100\text{nF}$ (104) | Radial P=2.5mm | 50V | 1 | Triệt tiêu xung nhiễu gợn sóng trên đường nguồn 5V |
| **KU1** | IC Ổn áp tuyến tính 3.3V | **AMS1117-3.3V** | SOT-223 / Chân dán | $V_{\text{in}} = 5\text{V}$, $V_{\text{out}} = 3.3\text{V}$, 800mA | 1 | Cấp nguồn 3.3V siêu ổn định và cách ly cho module NRF24L01 & cảm biến |
| **KC2 (3.3V rail)** | Tụ gốm lọc nguồn 3.3V | Tụ gốm $100\text{nF}$ (104) | Radial P=2.5mm | 50V | 1 | Lọc nguồn ngõ ra 3.3V của AMS1117 |
| **U9** | IC Đệm công suất | **ULN2003APG / ULN2003LVDR** | DIP-16 Chân Cắm | 50V, 500mA/kênh | 1 | Mảng Darlington khuếch đại dòng từ ESP32 kéo cuộn hút 6 rơ-le |
| **FWRL1 - FWRL6** | Rơ-le điện từ công nghiệp | **Omron G5LE-14 12VDC** (hoặc Songle SRD) | SPDT 5 Chân Through-Hole | Cuộn hút 12V DC, Tiếp điểm 10A 250VAC | 6 | Đóng ngắt tải: Bơm 1, Bơm 2, Phun sương, Quạt, Bơm tổng, Đèn |
| **FWD1 - FWD6** | Đi-ốt dập xung ngược | **1N4148** High Speed Fast Recovery | DO-35 Axial | 100V, 200mA, $t_{rr} = 4\text{ns}$ | 6 | Triệt tiêu xung điện áp phản kháng tự cảm khi ngắt cuộn hút rơ-le |
| **LED1 - LED6** | Đèn LED báo rơ-le | LED tròn 5mm (Đỏ) | Through-Hole 5mm | $V_f \approx 2.0\text{V}$, 15mA | 6 | Chỉ thị trạng thái BẬT/TẮT trực quan cho 6 rơ-le trên bo mạch |
| **R3 - R8** | Điện trở hạn dòng LED | $1\text{k}\Omega \pm 5\%$ | Axial 0.4W / RES10MM | $0.25\text{W}$ | 6 | Hạn dòng bảo vệ LED1 - LED6 |
| **FWU1 - FWU6** | Cọc đấu dây ngõ ra tải | Cọc cắm Domino 2 chân vít bước 5.08mm | Through-Hole DOMINO-2 | 300V 10A | 6 | Đấu nối ra máy bơm 1, 2, phun sương, quạt, đèn, van dinh dưỡng |
| **U6** | Cọc nguồn ngõ vào 12V | Cọc cắm Domino 2 chân vít bước 5.08mm | Through-Hole DOMINO-2P-5.08 | 300V 10A | 1 | Cấp nguồn nuôi 12V DC từ adapter hoặc nguồn tổ ong vào bo mạch |
| **R9, R10** | Trở phân áp bảo vệ ADC | $3.3\text{k}\Omega \pm 1\%$ | Axial RES10MM | $0.25\text{W}$ | 2 | Nhánh trên cầu phân áp giảm mức điện áp cảm biến 5V về dải ADC ESP32 |
| **R11, R12** | Trở phân áp bảo vệ ADC | $6.8\text{k}\Omega \pm 1\%$ | Axial RES10MM | $0.25\text{W}$ | 2 | Nhánh dưới cầu phân áp (tỉ lệ $\approx 0.673$, chuyển $5\text{V} \rightarrow 3.37\text{V}$) |
| **THOAT, TANG, GIAM, KEY2** | Phím bấm điều khiển HMI | Phím nhấn 4 chân công nghiệp $6\times 6\text{mm}$ | Through-Hole `K4-6x6_TH` | 12V 50mA | 4 | 4 phím điều hướng: THOÁT (GPIO 15), TĂNG (GPIO 2), GIẢM (GPIO 4), CHỌN (GPIO 16) |
| **RST** | Nút nhấn Reset phần cứng | Phím nhấn 4 chân công nghiệp $6\times 6\text{mm}$ | Through-Hole `K4-6x6_TH` | 12V 50mA | 1 | Nối chân EN của ESP32 kéo về GND để Reset nóng hệ thống |
| **R15 - R19** | Điện trở kéo lên phím bấm | $10\text{k}\Omega \pm 5\%$ | Axial RES10MM | $0.25\text{W}$ | 5 | Trở treo nguồn kéo mức logic HIGH cho 4 nút menu và nút Reset |
| **U3** | Module Màn hình hiển thị | **LCD1602 + PCF8574T I2C Backpack** | Khối hiển thị 16 ký tự x 2 dòng | 5V DC (Địa chỉ I2C: `0x27`) | 1 | Hiển thị thông số vi khí hậu và giao diện cài đặt ngưỡng trực quan |
| **U4** | Module Thu phát vô tuyến | **NRF24L01+ 2.4GHz Breakout** | Header cái 2x4 chân bước 2.54mm | 1.9V - 3.6V (Cấp từ KU1 AMS1117) | 1 | Thu nhận dữ liệu sóng vô tuyến từ các node đo độ ẩm ngoài vườn |
| **SE** | Cảm biến nhiệt ẩm không khí | **DHT11** Digital Sensor | Module 4-Pin Through-Hole | 3.3V - 5V DC (Chân DATA: GPIO 32) | 1 | Giám sát nhiệt độ không khí ($0-50^\circ\text{C}$) và độ ẩm ($20-80\%$) |
| **P1** | Cổng Cảm biến khí gas | Header 3 chân bước 2.54mm (VCC, DATA, GND) | Chân cắm đực 1x3 Pin | 5V DC (Ngõ ra AOUT qua cầu trở vào GPIO 34) | 1 | Kết nối module cảm biến chất lượng không khí **MQ-135** |
| **P2** | Cổng Cảm biến phao nước | Header 3 chân bước 2.54mm (`IN PHAO`) | Chân cắm đực 1x3 Pin | 3.3V / 5V DC (Chân tín hiệu nối GPIO 33) | 1 | Kết nối cảm biến phao cơ báo cạn/báo đầy bể nước tưới |
| **U2** | Cảm biến cường độ sáng | **GY-302 (BH1750FVI Digital Lux)** | Header 5 chân (VCC, GND, SCL, SDA, ADDR) | 3.3V - 5V DC (Địa chỉ I2C: `0x23`) | 1 | Đo quang thông ánh sáng mặt trời ($1 - 65,535\text{ Lux}$) |
| **U7** | Bo khuếch đại điện cực pH | **PH-4502C** Module | Module chân cắm + Cổng BNC cái | 5V DC (Ngõ ra Po qua cầu trở vào GPIO 35) | 1 | Khuếch đại vi sai tín hiệu mili-volt từ đầu dò thủy tinh đo pH nước |
| **Đầu dò pH** | Điện cực thủy tinh BNC | Que đo pH công nghiệp đầu cáp BNC | Cáp đồng trục dài 1m + BNC | Dải đo pH 0 - 14 | 1 | Cắm trực tiếp ngập trong bể dinh dưỡng hoặc đường ống hồi |

---

## 2. Bo Mạch Điểm Đo Cảm Biến Di Động (Remote Field Sensor Node)

Bo mạch Node cắm đất được thiết kế tối giản, siêu tiết kiệm năng lượng, vận hành bằng nguồn pin Li-ion độc lập và có khả năng mở rộng cắm đến 6 đầu dò cảm biến độ ẩm đất:

| Ký hiệu trên sơ đồ | Tên linh kiện | Model / Thông số kỹ thuật | Kiểu đóng vỏ (Package) | Điện áp & Dòng | Số lượng | Vai trò trong mạch Node |
| :---: | :--- | :--- | :---: | :---: | :---: | :--- |
| **U1** | Vi điều khiển AVR | **ATmega8A-PU / ATmega88V-10PU** | DIP-28 Chân Cắm | 2.7V - 5.5V (Chạy dao động nội 8MHz) | 1 | Đọc ADC que đo độ ẩm, đóng gói khung truyền vô tuyến RF |
| **U3** | IC Ổn áp nguồn pin | **AMS1117-3.3V** | SOT-223 Chân Dán | $V_{\text{in}} = 3.7-4.2\text{V}$, $V_{\text{out}} = 3.3\text{V}$ | 1 | Ổn định điện áp 3.3V cho ATmega8 và NRF24 khi pin xả |
| **C1** | Tụ lọc ngõ ra 3.3V | Tụ hóa / Tụ Tantalum $1\mu\text{F} / 16\text{V}$ | Through-Hole / SMD A | 16V | 1 | San phẳng điện áp 3.3V cấp cho NRF24L01 chống sụt áp tức thời |
| **C2** | Tụ lọc ngõ vào Pin | Tụ hóa / Tụ Tantalum $1\mu\text{F} / 16\text{V}$ | Through-Hole / SMD A | 16V | 1 | Ổn định áp đầu vào từ khối Pin Li-ion 18650 |
| **U2** | Module Thu phát sóng | **NRF24L01+ 2.4GHz Breakout** | Header cái 2x4 chân bước 2.54mm | 1.9V - 3.6V | 1 | Phát gói tin dữ liệu RF định kỳ về Gateway trung tâm |
| **P1 - P6** | Cổng cắm cảm biến đất | Header 3 chân 2.54mm (Pin 1: SIG, Pin 2: VCC, Pin 3: GND) | Chân cắm đực thẳng 1x3 Pin | 3.3V DC (Nối tương ứng ADC0–ADC5) | 6 | Mở rộng cắm tối đa 6 que đo độ ẩm điện dung chống ăn mòn |
| **U4** | Cổng nạp chương trình ISP | Header 6 chân bước 2.54mm (1x6 Pin) | Chân cắm đực 1x6 Pin | SCK, MISO, MOSI, RST, VCC, GND | 1 | Nạp Bootloader và Firmware cho ATmega8 qua mạch nạp USBasp |
| **LED2** | Đèn LED báo trạng thái | LED tròn 5mm (Đỏ) | Through-Hole 5mm | $V_f \approx 2.0\text{V}$, 10mA | 1 | Nối chân PD5 (Pin 11), nháy sáng khi gửi gói tin RF thành công |
| **R2** | Điện trở hạn dòng LED | **$220\Omega \pm 5\%$** | Axial 0.4W RES10MM | $0.25\text{W}$ | 1 | Hạn dòng bảo vệ LED2 trên chân PD5 |
| **H1** | Cọc nguồn Pin | Cọc Domino 2 chân vít bước 2.54 / 5.08mm | Through-Hole DOMINO-2 | 3.7V - 4.2V DC | 1 | Đấu nối vào đầu ra của mạch bảo vệ pin TP4056 |
| **Pin & Sạc** | Nguồn năng lượng độc lập | Cell Pin 18650 3.7V 2600mAh + Mạch TP4056 Type-C | Khối Pin + Module sạc Type-C | 3.7V xả, 4.2V ngắt sạc | 1 bộ / node | Cung cấp năng lượng tự vận hành ngoài ruộng nhiều tháng |
