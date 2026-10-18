# Assembly & Fabrication Guide

This guide details the manufacturing, 3D printing, soldering, and mechanical assembly procedures for the Smart Agriculture IoT System.

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
  - `PCB_Mach_Sever_Etch_Mask.pdf`
  - `PCB_ATMEGA8_NRF24_Etch_Mask.pdf`
- Print on high-density transfer paper or UV transparency film.
- Transfer onto single-sided copper clad board and etch with Ferric Chloride ($FeCl_3$) or Ammonium Persulfate.

### Option C: CAD Source Modification
- Open [`hardware/schematics/easyeda/`](file:///hardware/schematics/easyeda/) and [`hardware/pcb/easyeda/`](file:///hardware/pcb/easyeda/) in EasyEDA Standard/Pro.
- Or open Sprint-Layout source files in [`hardware/pcb/sprint_layout/`](file:///hardware/pcb/sprint_layout/) with Sprint-Layout 6.0.

---

## 2. Soldering & Assembly Sequence

### A. Central Gateway Board
1. **Low-profile passive components**: Solder all resistors (pull-ups for I2C and buttons) and 1N4007 diodes.
2. **Capacitors**: Solder 100nF ceramic decoupling capacitors close to IC pads.
3. **IC Sockets & Headers**:
   - Solder female 2.54mm pin headers for the ESP32 DevKit (allowing easy detachment).
   - Solder female 2x4 2.54mm header for the NRF24L01+ module.
4. **RF Decoupling**: Solder a **$10\mu\text{F} - 47\mu\text{F}$ electrolytic or tantalum capacitor** directly across Pin 1 (GND) and Pin 2 (VCC) of the NRF24 header on the bottom side of the PCB.
5. **Screw Terminals & Connectors**: Solder terminal blocks for 12V/5V DC power inputs and relay wiring headers.
6. **Voltage Regulators**: Solder AMS1117-3.3V with appropriate copper heatsink polygon.

### B. Wireless Sensor Nodes
1. Solder DIP-28 socket for ATmega8 (do not insert the microcontroller until power rail voltages are verified).
2. Solder 8MHz/16MHz crystal and two 22pF ceramic capacitors.
3. Solder 2x4 header for NRF24L01+ with direct $10\mu\text{F}$ bypass capacitor.
4. Solder 3-pin JST connector for the capacitive soil moisture sensor.
5. Solder the TP4056 module and connect the 18650 battery holder leads via an SPDT toggle switch.

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
