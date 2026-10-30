# Option 2 (custom Ø24 mm PCB) BOM: fact check

Checked on 2026-10-26 (Europe/Helsinki). Sources:
- JLCPCB parts-library API (selectSmtComponentList) for stock, library type and price.
- LCSC product pages (lcsc.com/product-detail/Cxxxx.html).
- Sensirion STCC4 datasheet (D1, June 2025) and STCC4 Design-In Guide.
- MCP73831 datasheet.
- Knowles SPH0641LU4H-1 datasheet.
- Fanstel BC840 and Insight SiP ISP2454 datasheets.
- Holyiot product listings.

Prices are JLC/LCSC USD unit prices at the 1–9 tier, which is what you pay at qty 5. Stock figures are a snapshot from the same day. All JLC parts are "extended" unless marked basic, and each extended part adds a loading fee per order (about $3 each from memory; not re-checked).

## Verdict

**Not orderable as written. 3 of the 9 LCSC numbers point to the wrong part, and the BLE module line is wrong in every respect.**

- The circuit idea is broadly sound: nRF52840 + STCC4 with SHT40 on the STCC4's own sensor bus + PDM mic + MCP73831 charger + LDO.
- **Ø24 mm is only realistic with an 8×8 mm module that JLC does not stock** (ISP1807 sourced elsewhere) and parts on both sides of the board.
- With what JLC actually has in stock, expect about **Ø26–28 × 9–10 mm**.
- **"€65 for 5 boards" is far off.** The STCC4 alone is $12.56 × 5 = $63. A realistic total for 5 assembled boards is about **$170–250** (estimate).

## Line-by-line

| # | Claimed | LCSC# given | What that # really is | Verdict |
|---|---|---|---|---|
| 1 | Holyiot-21014 (or Minew MS88SF2), "10×10 mm nRF52840 module with antenna" | C2688382 | **Raytac MDBT50Q-U1MV2**: nRF52840 with an **IPEX/u.FL connector** (needs an external antenna), QFN-61 10.5×15.5 mm. **Stock 0** at LCSC and JLC ($16.50). | ❌ Wrong in every respect. See below. |
| 2 | Sensirion STCC4-D-R3 | C49191471 | Correct. JLC lists STCC4-D-R3 with **1473 in stock**, extended, **$12.56** (1+) and $11.08 (10+). The lcsc.com product page returned "Page Not Found" today, but the JLC library has it. | ✅ |
| 3 | SHT40-AD1B-R2 | C2909967 | **XC6206P302MR, a 3.0 V LDO** (YONGYUTAI, SOT-23). Wrong part. | ❌ Correct number is **C2909890**: SHT40-AD1B-R2, DFN-4 1.5×1.5 mm, **19,351 in stock**, $1.92. |
| 4 | Knowles SPH0641LU4H-1 | C1927027 | **Knowles 0505Y0630271FQT, a 270 pF capacitor** (stock 0). Wrong part. | ❌ Correct number is **C2879853**: SPH0641LU4H-1, LGA-5 3.5×2.65×0.98 mm, **511 in stock**, $2.09. |
| 5 | MCP73831T-2ACI/OT | C424093 | Correct. SOT-23-5, **7,339 in stock**, $0.70. | ✅ Part is right, but see the charge-current note. |
| 6a | AO3401A | C15127 | Correct. P-MOSFET, SOT-23, **basic** part, 640k in stock, $0.096. | ✅ |
| 6b | 1N5819WS | C191023 | Correct. 40 V 1 A Schottky, SOD-323, **basic** part, 3.99M in stock, $0.012. | ✅ |
| 7 | MCP1700T-3302E/TT | C39051 | Correct. 3.3 V LDO, 1.6 µA quiescent current, 178 mV dropout at 250 mA, **16,454 in stock**, $0.48. | ✅ |
| 8 | ALPS SKRPACE010 4.2×3.2 mm | C139797 | Correct. SMD-4P 4.2×3.2×2.5 mm, **75,822 in stock**, $0.07. | ✅ |

### 1. BLE module: details

- **Holyiot-21014 is not a module.** It is a finished **nRF52810 beacon tag**, Ø30 × 8.4 mm, running on a CR2032 (Holyiot/AliExpress/Alibaba listings). Holyiot's nRF52840 module is the **Holyiot-18010**, 18 × 13.5 × 1.6 mm with a ceramic antenna, and JLC has none in stock.
- **Minew MS88SF2 is about 23.2 × 17.4 mm** (JLC listing), with 0 stock across all variants. It does not fit a Ø24 board.
- **No 10 × 10 mm nRF52840 module with an antenna is stocked at JLC today.** Real options:

| Module | SoC | Size | Antenna | JLC/LCSC stock | Notes |
|---|---|---|---|---|---|
| Insight SiP **ISP1807-LR-RS** | nRF52840 | **8×8×1 mm** LGA | integrated | C5264268: **0** at JLC ($12.22) | DigiKey/Mouser stock seen only in search snippets (≈9–15k, ≈$13.65): **unverified**. Would need JLC Global Sourcing or consignment. Fine-pitch LGA. |
| Insight SiP **ISP2454-LX** | nRF54L15 | 8×8×1 mm LGA | integrated, edge keep-out required | C44975104: **0** ($20.62) | Zephyr only. Integrates the 32 MHz crystal (32 kHz crystal on LX/LP). |
| Fanstel **BC840M** | nRF52840-CKAA | 7.1 (10.1 at antenna) × 12.3 × 1.5 mm | PCB trace | C5447804: **0** ($11.40) | Antenna must stick out about 5.5 mm past the ground plane. The smaller BC840 (7.1×9.2 mm) only reaches about 4–12 m. |
| Raytac **MDBT50Q-1MV2** | nRF52840 | about 10×15 mm (Raytac datasheet, not re-fetched) | chip antenna | C5118826: **0** ($19.07) | This is the variant the other AI probably meant, not "U" (IPEX). |
| Ebyte **E73-2G4M08S1C** | nRF52840 | 18×13 mm | PCB antenna | **C356849: 961 in stock, $7.69** | **The only nRF52840 module JLC actually stocks.** Pushes the board to about Ø26–28 mm. |
| Bare **nRF52840-QIAA-R** | — | aQFN-73 7×7 mm | you design it | C190794: 45,341 in stock, $5.96 | Smallest option. Needs your own RF matching, antenna, 32 MHz crystal, DC/DC inductors and 4 layers. **Not recommended for a first PCB.** |

### 3. Does SHT40 go to SDA_C/SCL_C? Yes, and that claim is correct.

From the STCC4 datasheet, §2.4–2.5:

- **Pinout:** pin 2 = **SDA_C**, pin 8 = **SCL_C**. This is a dedicated I²C *controller* port for an SHT4x at address **0x44**. SHT40-**A**D1B uses address 0x44.
- **Pull-ups:** both pins have internal 10 kΩ pull-ups, so don't add external ones when the SHT4x is close by.
- **Sampling:** the STCC4 reads the SHT4x itself once per second.
- **Alternative:** leave SDA_C/SCL_C on floating pads and have the MCU send T/RH every second with `set_rht_compensation` (0xE000). That costs MCU wake-ups, so the direct wiring is better.
- **Supply:** STCC4 VDD is **2.7–5.5 V** (typ. 3.3 V). I²C level is 1.8 V–VDD, set by VDDIO.

### 4. Microphone

- SPH0641LU4H-1 is a true 1-bit PDM, **bottom-port** (Ø0.325 mm port) mic, 1.62–3.6 V. A ~0.5–1 mm hole through the PCB under the port is correct.
- Cheaper candidates in stock at JLC:
  - **MSM261DCB002** C51928216: 1.9×2.8 mm, 5,665 in stock, $0.71.
  - **MSM261DDB020** C27636198: 2.7×3.5 mm, same footprint size as the SPH0641, 2,008 in stock, $0.65.
  - **ZTS6577** C18198782: 3,292 in stock, $0.89.
- All three are listed at −26 dB(FS), which is typical of digital mics. **Whether they are PDM, and whether the port is top or bottom, is unverified.** Check the datasheets before swapping.

### 5. Charger: is 20 kΩ about 50 mA?

**Yes.** The MCP73831 datasheet gives I_REG = 1000 V / R_PROG(kΩ), so 20 kΩ gives 50 mA.

But 50 mA is **1.25C for a 40 mAh cell**, above the usual 0.5–1C limit. Use **33 kΩ (≈30 mA)** or **47 kΩ (≈21 mA)**.

### 6. Load-sharing (AO3401A + 1N5819WS)

This is the standard Adafruit/SparkFun circuit and it works. It isn't essential here: the pendant only charges while off the neck, and its sleep current is in the µA range, far below the C/10 end-of-charge threshold, so charging will still terminate correctly.

Two options:
- **Keep it** (2 basic parts, about $0.11).
- **Drop it** to save about 15 mm² of board area. This is my recommendation for v0.

### 7. LDO

- MCP1700-3.3 is appropriate: 1.6 µA quiescent, and dropout at roughly 20 mA is only tens of mV.
- Below about 3.35 V the output simply tracks the battery. That is still fine: nRF52840 ≥1.7 V, mic ≥1.62 V, SHT40 ≥1.08 V, **STCC4 ≥2.7 V**.
- **Set the firmware cut-off at about 3.3–3.4 V** so the STCC4 stays in spec.
- Cheaper alternative that needs no extended-part fee: **XC6206P332MR-G, C5446**, basic part, 452k in stock, $0.145, listed at 1 µA.

## Other design problems

1. **Battery size.** A bare 401015 cell is 4×10×15 mm, but with the protection board (PCM) and leads it is **4×10×17 mm** (YDL; FP Battery datasheet). Sellers are China-based OEMs. **I found no EU shop stocking an exact 401015.** EU 40 mAh cells exist but in different sizes: Soldered 24×8.5×3.5 mm, Electrokit 20.5×10×3 mm. Make sure the cell has a PCM.
2. **STCC4 "air slot".**
   - The gas inlet is the membrane on the **top of the metal cap**, so no PCB hole is needed.
   - The Design-In Guide asks for **milled slots and narrow traces around the sensor for *thermal* decoupling**, plus an enclosure opening (a grille with several holes) and minimal air volume around the sensor.
   - Keep the sensor away from the LDO, charger and battery, and shield it from body heat and breath.
3. **CO2 every 5–10 min is outside the datasheet's specified operation.** Single-shot accuracy is only specified at a 10 s sampling interval. The automatic self-calibration (ASC) history is stored after 720 single shots, and after more than 3 h idle the sensor needs up to 1 h, or 360 shots, to regain accuracy. Expect lower accuracy, or run short bursts of single shots (my suggestion, untested).
4. **Antenna keep-out.** Every module listed needs its antenna at the board edge, with no copper on any layer under it (ISP2454/BC840 datasheets). This alone takes about 3–6 mm off one side of a Ø24 disc.
5. **Missing from the BOM:**
   - Charge input pads or pogo contacts (5 V, GND) and ESD/TVS protection on them.
   - 4.7 µF caps on the charger input and output (in the "passives" line only generically).
   - Battery pads and a **battery-voltage divider** (e.g. 1 MΩ/1 MΩ plus a cap) for the fuel gauge.
   - The recording LED and its resistor (the firmware uses one).
   - SWD pads: SWDIO, SWCLK, VDD, GND, and ideally RESET. E73 and BC840 expose SWD.
   - A 32.768 kHz crystal if the module lacks one (E73: unverified).
   - STCC4 ADDR tied to GND (0x64) and VDDIO tied to the rail.
6. **Programmer.** "XIAO Debug Mate" is unverified. Any J-Link, an nRF52 DK, or a CMSIS-DAP probe with a Tag-Connect or pogo jig will work.
7. **PCB.** 0.8 mm FR4. 4 layers if you use the bare SoC; 2–4 layers are fine with a module. Ø24 needs parts on both sides, and JLC charges extra for double-sided assembly.

## Corrected BOM (orderable at JLC today, E73 route)

| Role | Part | LCSC# | JLC stock | $/unit (qty 5) | Type |
|---|---|---|---|---|---|
| BLE MCU | Ebyte E73-2G4M08S1C (nRF52840, 18×13) — *or ISP1807-LR-RS via Global Sourcing for Ø24* | C356849 (ISP1807: C5264268) | 961 (ISP1807: 0) | 7.69 (ISP1807: 12.22) | ext |
| CO2 | STCC4-D-R3 | C49191471 | 1,473 | 12.56 | ext |
| T/RH | SHT40-AD1B-R2 (0x44) on SDA_C/SCL_C | **C2909890** | 19,351 | 1.92 | ext |
| Mic | SPH0641LU4H-1 (bottom port) | **C2879853** | 511 | 2.09 | ext |
| Charger | MCP73831T-2ACI/OT, **R_PROG 33–47 kΩ** | C424093 | 7,339 | 0.70 | ext |
| LDO | XC6206P332MR-G (or MCP1700T-3302E/TT C39051, $0.48) | C5446 | 452,019 | 0.145 | **basic** |
| PTT button | ALPS SKRPACE010 | C139797 | 75,822 | 0.07 | ext |
| Optional load-share | AO3401A + 1N5819WS | C15127 + C191023 | 640k / 3.99M | 0.11 | basic |
| Passives, LED, TVS | 0402 R/C, 0402 LED, ESD diode | JLC basic (pick at layout) | — | ≈0.30 | basic |
| Battery (buy separately) | 401015 40 mAh with PCM, 4×10×17 mm | — | China OEMs (e.g. YDL ≈$2.1) | ≈2–4 | — |

- **Parts per board:** about $25.5 with E73, or about $30 with ISP1807.
- **5 assembled boards:** about **$170–250** including PCB, assembly setup and extended-part fees (estimate).

## Estimated dimensions

These are my estimates from part footprints.

| Route | Board size | Total height |
|---|---|---|
| **ISP1807 (8×8), double-sided** | **Ø23–24 mm** | about 9–10 mm: 0.8 shell + ~1.2 top parts + 0.8 PCB + ~1 bottom parts + 4 battery + ~0.5 foam/clearance + 0.8 shell |
| **E73 (18×13), JLC-stocked** | **Ø26–28 mm** | about 9–10 mm |
| **Bare nRF52840 + chip antenna** | Ø22–24 mm | about 9–10 mm, but needs an RF design. Not advised for v0. |
