# Assembly, Fabrication & Soldering Guide

This guide details the manufacturing, 3D printing, soldering, and mechanical assembly procedures for the Smart Agriculture IoT System, aligned 100% with the official schematics.

---

## 1. PCB Manufacturing Options

The repository includes complete production files for both industrial PCB fabrication and DIY rapid-prototyping etching:

### Option A: Professional PCB Fabrication (JLCPCB, PCBWay, etc.)
- Use the standard RS-274X Gerber archives in [`hardware/pcb/gerber/`](file:///hardware/pcb/gerber/):
  - `gateway/`: Contains `_copper_bottom.gbr`, `_outline.gbr`, `khoan.DRL` (Excellon drill).
  - `sensor_node/`: Contains `LLAY_copper_bottom.gbr`, `LLAY_outline.gbr`, `KHOAN.DRL`.
- Specification: Single or 2-layer FR-4, 1.6mm board thickness, 1oz copper, HASL finish.

### Option B: DIY Thermal Transfer / Photoresist Etching
- Open the 1:1 scale mirror/inverted PDFs in [`hardware/pcb/etch_masks_pdf/`](file:///hardware/pcb/etch_masks_pdf/):
  - `PCB_Mach_Sever_Etch_Mask.pdf` (Central Gateway Board)
  - `PCB_Sensor_Node_ATmega8_Etch_Mask.pdf` (Remote Sensor Node Board)
- Print on high-density transfer paper or UV transparency film.
- Transfer onto single-sided copper clad board and etch with Ferric Chloride ($FeCl_3$) or Ammonium Persulfate.

### Option C: CAD Source Modification
- Open [`hardware/schematics/easyeda/`](file:///hardware/schematics/easyeda/) and [`hardware/pcb/easyeda/`](file:///hardware/pcb/easyeda/) in EasyEDA Standard/Pro.
- Or open Sprint-Layout source files in [`hardware/pcb/sprint_layout/`](file:///hardware/pcb/sprint_layout/) with Sprint-Layout 6.0.

---

## 2. Soldering & Assembly Sequence

### A. Central Gateway Board
1. **Low-profile passive components**:
   - Solder pull-up resistors $R_{15} - R_{19}$ ($10\text{k}\Omega$) for the 4 menu buttons and hardware reset.
   - Solder ADC voltage divider resistors $R_9, R_{10}$ ($3.3\text{k}\Omega$) and $R_{11}, R_{12}$ ($6.8\text{k}\Omega$).
   - Solder relay LED current-limiting resistors $R_3 - R_8$ ($1\text{k}\Omega$).
   - Solder 6x freewheeling diodes $FWD_1 - FWD_6$ (**1N4148**), ensuring correct cathode stripe orientation.
2. **Capacitors & Filter Inductor**:
   - Solder high-frequency decoupling ceramic capacitors $C_1, C_2$ ($0.1\mu\text{F}$) and $KC_2$ ($100\text{nF}$).
   - Solder LM2596 filter inductor $L_1$ ($1\text{mH}$ toroidal / radial).
   - Solder electrolytic capacitors $KC_1$ ($10\mu\text{F} / 25\text{V}$), verifying polarity.
3. **IC Sockets & Connectors**:
   - Solder DIP-16 socket for the **ULN2003APG** Darlington driver.
   - Solder female 2.54mm header strips for the ESP32 DevKit (38-pin), allowing easy detachment.
   - Solder female 2x4 2.54mm socket for the NRF24L01+ transceiver.
   - Solder 4-pin female header for DHT11 and 3-pin headers for MQ-135 ($P_1$) and Float Switch ($P_2$).
   - Solder 5-pin I2C header for the GY-302 BH1750 ambient light sensor.
   - Solder header pins for the LCD 1602 I2C backpack and the PH-4502C signal conditioning board.
4. **Power Regulators & Relays**:
   - Solder IC **LM2596-5.0** ($U_5$) switching buck regulator.
   - Solder IC **AMS1117-3.3V** ($KU_1$) linear LDO regulator.
   - Solder the 6 onboard **Omron G5LE-14 12VDC** SPDT relays ($FWRL_1 - FWRL_6$).
   - Solder 6x 5mm Red indicator LEDs ($LED_1 - LED_6$).
   - Solder the 4 tactile menu buttons (`THOAT`, `TANG`, `GIAM`, `KEY2`/`CHON`) and `RST` button.
   - Solder all 7 **DOMINO-2P-5.08** terminal blocks: $U_6$ (12V Input) and $FWU_1 - FWU_6$ (Relay Outputs).

### B. Wireless Remote Sensor Nodes
1. Solder DIP-28 socket for the **ATmega8 / ATmega88** (the chip operates on its calibrated **Internal 8MHz RC Oscillator**, so no external crystal or 22pF load capacitors are required).
2. Solder IC **AMS1117-3.3V** ($U_3$) along with input filter capacitor $C_2$ ($1\mu\text{F}$) and output filter capacitor $C_1$ ($1\mu\text{F}$).
3. Solder female 2x4 header for the NRF24L01+ module.
4. Solder male 1x6 ISP header ($U_4$) for in-system firmware programming via USBasp.
5. Solder the 6x 3-pin headers ($P_1 - P_6$) for connecting capacitive soil moisture probes to ADC channels A0–A5.
6. Solder status indicator $LED_2$ (Red 5mm) and current-limiting resistor $R_2$ ($220\Omega$) on pin PD5.
7. Solder battery input connector $H_1$ (DOMINO-2 / 2-pin JST) connecting to the TP4056 protected 18650 Li-ion cell.

---

## 3. 3D Printing Specifications

All 3D enclosure and bracket models are located in [`mechanical/3d_models/`](file:///mechanical/3d_models/).

| Part Name | File | Recommended Material | Infill | Layer Height | Supports |
| :--- | :--- | :---: | :---: | :---: | :---: |
| **Gateway Enclosure Box** | `Gateway_Box.stl` / `.3mf` | PETG or PLA | 20% - 25% | 0.20 mm | None |
| **Gateway Snap Lid** | `Gateway_Lid.stl` / `.3mf` | PETG or PLA | 20% | 0.20 mm | None |
| **BH1750 Sensor Body** | `BH1750_Body.stl` | White/Light PETG | 30% | 0.16 mm | Minimal |
| **BH1750 Diffuser Lid** | `BH1750_Lid.stl` | Transparent PLA/PETG | 100% | 0.12 mm | None |
| **BH1750 Angle Bracket** | `De_Cam_Bien_Anh_Sang.stl` | PETG | 30% | 0.20 mm | Optional |
| **MQ Gas Sensor Hood** | `MQ_Gas_Sensor_Cover.stl` | PETG / ABS | 25% | 0.20 mm | Yes |
| **Node Base Enclosure** | `RF-nano_case_BOTTOM.stl` | PETG | 25% | 0.20 mm | None |
| **Node Top Enclosure** | `RF-nano_case_TOP.stl` | PETG | 25% | 0.20 mm | None |

> [!TIP]
> For outdoor and greenhouse deployments, **PETG or ASA/ABS** is strongly recommended over standard PLA due to thermal stability (greenhouses can exceed 50°C in direct summer sun) and UV resistance.

---

## 4. Waterproofing & Field Deployment

1. **Capacitive Soil Sensor Sealing**:
   - The electronics on the upper portion of the capacitive soil probe (timer IC and resistors) must be sealed against moisture and morning dew.
   - Dip the top 20mm of the sensor PCB in clear polyurethane resin or seal thoroughly with marine-grade silicone. **Leave the lower probe area uncoated.**
2. **Cable Glands**:
   - Use PG7 or PG9 waterproof cable glands for power cables entering the Gateway enclosure.
3. **Node Enclosure Seal**:
   - Apply a bead of silicone grease along the perimeter groove of the wireless node enclosure before closing the lid.
