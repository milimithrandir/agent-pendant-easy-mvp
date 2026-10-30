# Agent Pendant, Option 2: start-to-finish build plan (Grok Bot version)

> **Status 2026-10-26: Max chose the MINIMUM PATH → follow [`MINIMUM_PATH.md`](MINIMUM_PATH.md).**
> This file is now the **recommended / extended alternative**: 5 boards, nRF52840 DK, PPK2, 2–3 case prints, ≈ €400–550. Its architecture, schematic, layout rules and bring-up checklist still apply.
> The minimum path overrides these points:
> - 2 boards;
> - Pi Debug Probe only;
> - TP4054 charger (C32574) instead of MCP73831;
> - 0603 basic LED;
> - ESD diode not fitted;
> - OSP finish preferred over ENIG;
> - 2 back pads by default;
> - 1 case print.

Checked 2026-10-26 (Europe/Helsinki). Nothing has been ordered or tested on hardware.

**How to read the labels**
- **Prices and stock** come from the live JLCPCB parts-library API on 2026-10-26: USD, quantity 1–9 tier. They change daily.
- **(est.)** means my estimate. It is not a vendor figure.
- **(unverified)** means I couldn't confirm it from a primary source today.
- **(snippet)** means the figure comes from a search-result summary, not from the page itself.
- **Currency:** €1 = $1.1188 (ECB reference rate, 2026-10-26), so $1 ≈ €0.894.

Companion file: `option2_bom.csv`. It supersedes the Path 2 BOM in `LATEST_SPEC.md` and in `option2_bom_factcheck.md`.

---

## 0. Headline

| | Value |
|---|---|
| Module | **u-blox BMD-340-A-R** (nRF52840, 15.0 × 10.2 × 1.9 mm, integrated antenna, FCC/IC/CE/MIC/KCC certified). JLC part C5456944: **416 in stock, $9.74** |
| Board | **Ø26 mm round, 4-layer, 0.8 mm, single-sided assembly.** Ø24 fits geometrically, but only with about 0.3 mm margin, so it's a later stretch goal. |
| Finished pendant | **about Ø28.5 × 8.5 mm** with a 40 mAh 3 mm cell; **about Ø28.5 × 10.5 mm** with the 55 mAh 5 mm cell (est.) |
| Parts per board | **about $29.4 (≈ €26)** |
| Minimum order (2 assembled boards) | **about €150–195 delivered to Finland** (est.) |
| Recommended order (5 boards) | **about €255–315 delivered** (est.) |
| Ten boards | about €440–500 (est.) |
| Battery life | about 3–4 weeks at 10 min of talk per day, about 1 week at 60 min per day (est.) |
| Time to a wearable prototype | **about 5–6 weeks** (est.) |
| Tools Max buys once | about €80 essential (nRF52840 DK €64.87 gross at TME FI, plus the Raspberry Pi Debug Probe €11.90), plus optional PPK2 €129.75 |

---

## 1. Architecture and module choice

### 1.1 Block diagram

```
 back pads: VBUS, GND (+ optional D+, D-) for a magnetic pogo cable
      |
   ESD diode -> MCP73831 (R_PROG 47k = 21 mA) -> LiPo 40 mAh with PCM (BAT+/BAT- pads)
                     | STAT -> GPIO + LED                 |
                                                          v
                                    XC6206 3.3 V LDO (1 uA Iq) -> 3V3 rail
                                                          |
   +------------------------------------------------------+-------------------------+
   | BMD-340 (nRF52840): LV mode, VCC = VCCH = 3V3, on-module DC/DC, 32 MHz XO       |
   |   + 32.768 kHz crystal (C32346) on P0.00/P0.01                                  |
   |   PDM CLK/DATA <- top-port PDM mic. Mic VDD switched by the PTT button (P-FET)  |
   |   I2C SDA/SCL -> STCC4 (0x64) --SDA_C/SCL_C--> SHT40 (0x44, on the STCC4 bus)  |
   |   GPIO: PTT button, LED, CHG_STAT, VBUS_DET, VBAT ADC (1M/1M divider)           |
   |   SWD pads: VDD, GND, SWDIO, SWCLK, RESET (P0.18), + VBAT pad for bench power   |
   +-------------------------------------------------------------------------------+
```

Design choices:
- **No USB-C.** Back pads plus a magnetic pogo cable keep the pendant 2–3 mm thinner and look like jewelry.
- **No vibration motor and no privacy slide switch.** The PTT button itself powers the mic, so privacy is enforced in hardware (§3.2).
- **No load-sharing circuit.** The device charges off the neck.

### 1.2 Module comparison (JLC stock checked today)

| Module | Size (mm) | Antenna | 32 kHz crystal | JLC part / stock / $ | Certification | Verdict |
|---|---|---|---|---|---|---|
| **u-blox BMD-340-A-R** | **15.0 × 10.2 × 1.9**, LGA-60 | integrated PCB antenna | optional (internal RC or external crystal) | **C5456944 / 416 / $9.74** | FCC 2AA9B10, IC 12208A-10, CE (RED), RCM, MIC, KCC, Bluetooth QDID 95452 | **Pick.** Smallest certified module JLC can place today. On-module DC/DC and 32 MHz crystal, 4.7 µF on VCC/VCCH. |
| Ebyte E73-2G4M08S1C | 18 × 13, castellated "stamp holes" | ceramic antenna | **must be added externally** (Ebyte manual: "32.768KHz crystal oscillator needs external connection") | C356849 / 909 / $7.69 | not verified for this variant | **Fallback.** About 2 mm larger board, but hand-solderable. Height: PR #2 says 3.0 mm (unverified here). |
| Raytac MDBT50Q-1MV2 / P1MV2 | about 10.5 × 15.5 | chip antenna | — | C5118826 / **0** / $19.07; C5119772 / **0** / $7.97 | FCC/CE (Raytac, unverified today) | Not stocked |
| Insight SiP ISP1807-LR-RS | **8 × 8 × 1** | in-package | — | C5264268 / **0** / $12.23 | — | Best for a future Ø22–24 v2, via JLC Global Sourcing (9–15 days) |
| Fanstel BT840 / BT840E / BC840M | various | PCB | — | C5182373 / C5162133 / C5447804: **all 0** | — | Not stocked |
| Minew MS88SF2 / MS88SF3 | 23.2 × 17.4 / 18.5 × 12.5 | — | — | C2836129 / C20416747: **0** | — | Not stocked, and too big |
| Holyiot-18010 | 18 × 13.5 | ceramic antenna | — | C9900042076: **0** | — | Not stocked |
| Bare nRF52840-QIAA (aQFN-73, 7 × 7) | — | your own design | your own design | C190794 / 45,282 / $5.96 | none | Not for a first board: needs RF matching, an antenna, a 32 MHz crystal and 0.5 mm-pitch aQFN |

**Why BMD-340 over E73**
- It is smaller in area: 153 mm² vs 234 mm², and 1.9 mm tall.
- It is fully pre-certified.
- The DC/DC inductors are on the module.
- It costs only $2 more.

**Downside:** the pads are underneath (LGA), so it can't be hand-soldered without hot air. That's fine because JLC places it and X-rays it.

**EU availability:**
- DigiKey shows about 11k in stock but only as 1000-piece reels; Mouser shows 0 (snippet, findchips/octopart).
- For small quantities, **JLC is the practical source.** Small quantities in the EU are not realistic.
- Sources:
  - BMD-340 datasheet: https://content.u-blox.com/sites/default/files/BMD-340_DataSheet_UBX-19033353.pdf (R13; type BMD-340-A-R-10 is "Mass production")
  - LCSC: https://www.lcsc.com/product-detail/C5456944.html
  - E73 manual: https://jlcpcb.com/api/file/downloadByFileSystemAccessId/8612736116701077504 and https://www.cdebyte.com/products/E73-2G4M08S1C

### 1.3 Certification (personal prototype)

CE marking and FCC rules apply when a product is *placed on the market*, meaning sold or given away. A one-off device built for personal use is generally not that, though it still must not cause interference. This is my understanding, not legal advice.

The BMD-340 helps here:
- **EU:** it already complies with RED 2014/53/EU.
- **US:** it carries a modular FCC grant, as long as its antenna and RF section are unmodified.
- A later product would then mainly need EMC (unintentional radiator), safety and battery testing, not full radio certification. Building on the bare nRF52840 would forfeit all of this.

---

## 2. BOM

### 2.1 Parts assembled by JLC (Economic PCBA)

Full list with source URLs: `option2_bom.csv`.

"Type" is JLC's library class:
- **base** parts carry no loading fee;
- **ext** (extended) parts cost **$3.07 each per order** on Economic PCBA.

| Ref | Function | Part / MPN | LCSC | Stock | $/pc | Type |
|---|---|---|---|---|---|---|
| U1 | BLE SoC module | u-blox BMD-340-A-R | C5456944 | 416 | 9.74 | ext |
| U2 | CO₂ sensor | Sensirion STCC4-D-R3 | C49191471 | 1,473 | 12.56 | ext |
| U3 | T/RH for STCC4 compensation | Sensirion SHT40-AD1B-R2 (0x44) | C2909890 | 18,521 | 1.92 | ext |
| MK1 | PDM mic, **top port** | Knowles SPK0838HT4H-1 (4 × 3 × 1 mm) | C5447637 | **37** (low) | 3.86 | ext |
| MK1-alt | PDM mic, alternative | Infineon IM69D130V01 (4 × 3 × 1.2 mm) | C536262 | 246 | 4.09 | ext |
| U4 | LiPo charger | Microchip MCP73831T-2ACI/OT | C424093 | 6,745 | 0.70 | ext |
| U5 | 3.3 V LDO, 1 µA Iq | Torex XC6206P332MR-G | C5446 | 445,692 | 0.15 | **base** |
| Q1 | Mic power switch (PTT-gated) | AO3401A P-MOSFET | C15127 | 709,207 | 0.10 | **base** |
| SW1 | Push-to-talk, 5.1 × 5.1 × **1.5 mm** | XKB TS-1187A-B-A-B | C318884 | 1,188,048 | 0.02 | **base** |
| Y1 | 32.768 kHz, 12.5 pF, 3.2 × 1.5 | Q13FC13500004 (FC-135) | C32346 | 489,634 | 0.17 | **base** |
| D1 | ESD on VBUS pad | LESD5D5.0CT1G, SOD-523 | C5274290 | 88,901 | 0.02 | ext |
| LED1 | Rec/charge indicator | NCD0402R1, 0402 red | C130719 | 533,643 | 0.04 | ext |
| C | 100 nF ×4, 1 µF ×2, 4.7 µF ×3, 10 µF ×1, 20 pF ×2 (crystal) | C1525, C52923, C23733, C15525, C1554 | | millions | ≈0.06 total | base |
| R | 47k R_PROG, 1M ×2 (VBAT divider), 100k ×3 (FET gate, VBUS divider), 1k (LED), 10k ×2 (I²C pull-ups) | C25792, C26083, C25741 (100k), C11702, C25744 | | millions | ≈0.02 total | base |

**Per board: ≈ $29.35 in parts.** Seven unique extended parts add $21.49 per order.

Notes:
- **Mic choice.**
  - SPK0838HT4H-1 is a verified top-port PDM mic. Datasheet: https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/694/SPK0838HT4H-1.pdf.
  - Only 37 are in stock. If it runs out, use IM69D130. A search summary says the IM69D130 is top-port, but **check the port side in its datasheet drawing before layout (unverified).**
  - The previously chosen **SPH0641LU4H-1 (C2879853) is bottom-port.** On a single-sided board its sound hole would face the chest, so I dropped it unless we go double-sided.
- **Which LDO.**
  - XC6206 3.3 V is a free base part.
  - Below a battery voltage of about 3.35 V, the rail simply follows the battery. That's fine: the STCC4 minimum is 2.7 V, and its ripple must stay ≤ 30 mV per the datasheet, which the LDO helps with.
  - A 3.0 V version (C9972) is extended. It isn't needed.
- **Charger.** MCP73831: I_REG = 1000 V / R_PROG (kΩ).
  - **47 kΩ gives 21 mA**, i.e. 0.5C on 40 mAh. Electrokit's 40 mAh cell is rated 1C max = 40 mA.
  - 33 kΩ gives 30 mA, for the 55 mAh cell.
  - The MCP73831 has **no thermistor input**, so never charge it while worn.
- **I²C pull-ups (10k).** The nRF's internal pull-ups (about 13 kΩ) may be enough. Fit 10k, or leave them as DNP if space is short.
- **Mic power.** Mic VDD runs through Q1, and its gate is pulled low by SW1.
  - **The mic is physically unpowered unless the button is held**, whatever the firmware does.
  - SW1 also goes to a GPIO, so the firmware knows when to start PDM.
  - Mic power-up is ≤ 50 ms (Knowles datasheet, SPH0641 family), so the first ~50 ms of speech may clip.
  - Add a 0 Ω DNP option so a GPIO can power the mic instead, the way the XIAO does.
- **Cheaper and fewer extended parts.** Dropping D1 and LED1 saves $6 per order. I advise keeping both: the LED is the visible "mic is on" privacy cue.

### 2.2 What Max buys separately

| Item | Recommended | Price | Source |
|---|---|---|---|
| LiPo with PCM, thin | Electrokit 40 mAh **20.5 × 10 × 3 mm**, 1C max | 55 SEK | https://www.electrokit.com/en/battery-lipo-3.7v-40mah-bec (checked earlier today, in `mvp_feasibility.md`) |
| LiPo, more capacity | Electrokit 55 mAh 15 × 10 × **5 mm** (case +2 mm) | 55 SEK | https://www.electrokit.com/en/batteri-lipo-3.7v-55mah |
| LiPo, China alternative | 401015 (≈4 × 10 × 17 incl. PCM) or 301020 40 mAh | ≈ $2–4 (snippet) | AliExpress/OEM. Check that it has a PCM. |
| SWD programmer + dev board | **Nordic nRF52840 DK.** Its J-Link OB programs external boards through the debug-out header, and it's a full nRF52840 board for porting the firmware *before* the PCBs arrive. | **€51.90 net / €64.87 gross** | TME FI https://www.tme.eu/fi/details/nrf52840-dk/tiedonsiirtopaketit/nordic-semiconductor/ (snippet). The DevZone thread confirms external programming: https://devzone.nordicsemi.com/f/nordic-q-a/128970/ |
| Cheap SWD alternative | Raspberry Pi Debug Probe (CMSIS-DAP; OpenOCD `nrf52_recover` works) | €11.90 incl. 19% VAT | https://www.welectron.com/Raspberry-Pi-Debug-Probe_1 (snippet); recovery how-to: https://marceloborges.dev/posts/1/ |
| Alternative | SEGGER J-Link EDU Mini, **non-commercial use only** | $76 (snippet) | https://shop-us.segger.com/product/j-link-edu-mini-8-08-91/ |
| Current measurement (optional, recommended) | Nordic Power Profiler Kit II | €106.35 net / €129.75 gross | TME (snippet) |
| Pogo programming jig | 6 × P75-series spring pins (1.27 mm pads) + a 3D-printed clamp, **or** solder 30 AWG wires to the pads for the first boards | ≈ €5–10 (est.) | AliExpress/LCSC (C2826549-type pogos are on JLC, $0.08) |
| Charging cable | Magnetic 2- or 4-pin pogo cable (USB-A/C to pins), matched to the pad pitch | ≈ €5–10 (est., unverified) | AliExpress. Buy first, then copy its pin pitch into the PCB pads. |
| Hand tools | fine-tip iron (e.g. Pinecil), flux, Kapton tape, tweezers, multimeter; **no hot-air station needed** (factory assembly) | ≈ €40–60 (est.) | local/EU |
| Optional bench helper | Adafruit 6478 STCC4 + SHT41 breakout, to run the CO₂ code on the DK before the PCBs arrive | €24.49 excl. VAT (Kiwi, earlier today) | https://www.kiwi-electronics.com/en/adafruit-stcc4-and-sht41-co2-temperature-amp-humidity-sensor-stemma-qt-qwiic-20617 |

---

## 3. PCB design workflow

### 3.1 Tool: KiCad 10 (current stable 10.0.7, released 2026-10-07)

**Use KiCad, not EasyEDA, because:**
- KiCad files are plain text and diffable, which matters for the "Grok Bot version" repo and for AI-to-AI comparison.
- `kicad-cli` can run ERC/DRC and export Gerbers/BOM/CPL headless, which is CI-able.
- Plugins:
  - **Fabrication Toolkit** (bennymeg) exports JLC-format Gerber/BOM/CPL in one click;
  - **easyeda2kicad** pulls LCSC footprints and 3D models by C-number.
- **EasyEDA Pro** is the easier choice if Max wants to click-order: the LCSC library and JLC ordering are built in. It's less portable.
- Sources: https://www.kicad.org/blog/2026/10/KiCad-10.0.7-Release/, https://www.kicad.org/blog/2026/03/Version-10.0.0-Released/

### 3.2 Schematic, block by block (key values)

1. **Charge input:**
   - Pads VBUS and GND; optional D+ and D− pads to the BMD-340 USB pins for UF2/serial over the pogo cable.
   - D1 ESD diode on VBUS.
   - 4.7 µF on the charger input.
   - VBUS → 100k/100k divider → GPIO (VBUS_DET; scales 5 V to 2.5 V).
   - BMD-340 **VBUS pin (66)** connects to the pad VBUS if the USB data pads are used.
2. **Charger (MCP73831-2ACI):**
   - VDD = VBUS; VBAT → BAT+ pad.
   - 4.7 µF on VBAT.
   - **R_PROG 47 kΩ** (21 mA).
   - STAT → LED + 1k to VBUS, and to a GPIO with the internal pull-up enabled only while charging (tri-state output).
3. **LDO (XC6206P332):**
   - IN = BAT+, with 1 µF in and 1 µF out (Torex typical). Add 10 µF bulk on 3V3 for the BLE TX bursts.
4. **BMD-340:**
   - **LV mode: VCC (pin 17) and VCCH (pin 65) both to 3V3** (datasheet §3.3).
   - The module has 4.7 µF internally. Add 100 nF + 4.7 µF next to the pins.
   - **Do not use HV mode.** It would feed the battery straight in, but REG0 is limited to 5 mA for external loads (datasheet) and the STCC4 peaks at 4.2 mA.
   - **32.768 kHz crystal** on P0.00/P0.01 with **2 × 20 pF**. Calculation: C = 2·C_L − C_pin − C_stray = 2·12.5 − 4 − ~1 ≈ 20 pF; C_pin 4 pF per datasheet Table 11. Keep it within 3 mm of the pads.
   - SWD pads on SWDIO (44), SWCLK (43), RESET = P0.18 (39), VDD, GND.
5. **Suggested pin map** (all GPIOs are free-assignable through the nRF pin mux; finalise at layout):

| Signal | nRF pin | Why |
|---|---|---|
| PDM_CLK / PDM_DIN | P0.26 / P0.27 | full-drive pins (not "<10 kHz only") |
| I2C SDA / SCL | P0.04 / P0.05 | full-drive |
| PTT button (also gates Q1) | P0.11 | wake-on-low, sense |
| LED | P0.13 | |
| VBAT ADC (1M/1M + 100 nF) | P0.29 / AIN5 | analog pin (low-frequency OK) |
| CHG_STAT | P0.30 | slow |
| VBUS_DET | P0.31 / AIN7 | slow |
| USB D+/D− (optional) | dedicated | |

6. **Mic:**
   - VDD from Q1 drain, plus 100 nF.
   - SELECT → GND (left channel).
   - CLK and DATA routed short, with an optional 33 Ω series resistor on CLK.
   - **Top-port mic: no PCB hole needed.** The port faces the case front.
7. **STCC4 + SHT40:**
   - STCC4: VDD and VDDIO to 3V3 (VDDIO must be present at power-up per the datasheet). ADDR → GND (0x64). SDA/SCL to the MCU. SDA_C/SCL_C → SHT40 (0x44), which has internal 10k pull-ups, so add none.
   - 100 nF on STCC4 VDD, typical practice (the datasheet table I extracted lists no cap value).
   - The SHT40 sits within about 3 mm of the STCC4 on the same thermal island.
8. **Battery pads:** BAT+ and BAT− on the back, for the cell's leads, which are soldered.

### 3.3 Layout rules

- **Stackup:** 4 layers, **0.8 mm**, 1 oz outer layers, **ENIG** (flat pads for the LGAs). L2 is a solid GND plane, L3 carries 3V3 and routing.
  - JLC offers a 0.8 mm 4-layer controlled stackup (JLC7628 family).
  - The JLC advert says "4-layer from $2 for 5 pcs (50 × 50)". 0.8 mm + ENIG costs more: quote it on the day.
- **Why 4 layers and not 2:** the BMD-340 can be fanned out on 2 layers (datasheet Fig. 6), but on a Ø26 disc 4 layers give a clean ground and easy routing.
- **Antenna:**
  - The BMD-340 sits at the board edge with the **antenna end facing outward**.
  - **No copper on any layer** under or beyond the antenna end.
  - No battery, screw or metal bail near it. Put the antenna at the **bottom** of the pendant, away from the chain.
- **Vias under the module are tented** (datasheet §2.3.2).
- **STCC4 thermal island:**
  - Place the STCC4 + SHT40 on a peninsula at the board edge.
  - **Mill a slot (≥ 1.0 mm wide; JLC's minimum non-plated slot width is unverified) around about 270°.** Only a narrow neck carries the 5 signal traces.
  - Keep it at least 6 mm from the module and the charger.
  - Follow the Sensirion STCC4 Design-In Guide (thermal decoupling, minimal air volume, enclosure opening).
- **Mic:** near the front centre, with the port under the case's mic hole. A gasket ring seals the mic to the case so sound doesn't leak into the cavity.
- **Back side:** no components. The battery covers the middle.
  - 2 (or 4) charge pads, **1.5–2 mm wide gold pads at the pogo-cable pitch.**
  - Battery pads.
  - SWD pads, 1.0 mm round, 1.27–2.0 mm pitch, outside the battery footprint, or accessible before the battery is fitted.
- **Silkscreen** off the pads; 2–4 fiducials. JLC may add **edge rails or a panel** for small round boards. Accept the rails or panelize 2 × 2 with mouse-bites (unverified for Economic; the quote page will say).

### 3.4 Realistic minimum board size (worked from datasheet dimensions)

Geometry: the module's 10.2 mm edge (antenna end) lies on the circle; the 20.5 × 10 mm battery sits on the back, below the antenna zone. I assume the antenna zone is the last ~4.5 mm of the module (est.).

| Board Ø | Module far corner | Battery fit (needs chord ≥ 21.5 for a 20.5 cell + 0.5 margin) | Top-side area left after module | Verdict |
|---|---|---|---|---|
| 24 | fits; antenna edge 10.9 mm from centre | needs \|y\| ≤ 5.3 mm for a 10 mm-tall cell: **0.3 mm margin** | ≈ 300 mm² | Possible, with a 401015/17 mm cell and very tight routing. **Stretch goal for v2.** |
| **26** | fits | \|y\| ≤ 7.3 mm: **comfortable** | ≈ 378 mm² | **Recommended v1** |
| 28 | fits | easy | ≈ 463 mm² | E73 fallback size |

**Top-side parts need about 160 mm² in total (est.):**

| Part | Area |
|---|---|
| STCC4 island with slot, 8 × 7 | 56 mm² |
| Mic + gasket keep-out | ≈ 15 mm² |
| Button | 26 mm² |
| Charger | ≈ 12 mm² |
| LDO | ≈ 10 mm² |
| Crystal | ≈ 8 mm² |
| ~25 0402 parts + LED + ESD | ≈ 35 mm² |

### 3.5 Thickness stack (Ø26 board, est.)

| Layer | 40 mAh 3 mm cell | 55 mAh 5 mm cell |
|---|---|---|
| Front wall (resin) | 1.0 | 1.0 |
| Clearance | 0.2 | 0.2 |
| Tallest top part (BMD-340, 1.9) | 1.9 | 1.9 |
| PCB | 0.8 | 0.8 |
| Battery + 0.3 swell/foam | 3.3 | 5.3 |
| Back wall | 1.0 | 1.0 |
| **Total** | **≈ 8.2 → 8.5 mm** | **≈ 10.2 → 10.5 mm** |

**Outer diameter** = 26 + 2 × (0.2 clearance + 1.0 wall) ≈ **Ø28.5 mm**, plus the bail.

For scale:
- Limitless Pendant: about 31.9 × 16 mm.
- 2 € coin: Ø25.75 × 2.2 mm.

---

## 4. Manufacturing (JLCPCB)

### 4.1 Facts (verified today)

Sources:
- https://jlcpcb.com/help/article/pcb-assembly-price (updated 2026-10-22)
- https://jlcpcb.com/solutions/pcb-prototype-assembly

**Minimums and service tiers**
- **The minimum PCBA order is 2 boards.** Argon's "2-board minimum" is real.
- **Economic PCBA** covers 2–50 pcs. It supports 0402 and larger, 0.4 mm IC pitch, LGA/QFN, and 2/4/6 layers. Build time is **1–3 days**.

**Economic PCBA fees**

| Fee | Amount |
|---|---|
| Setup | **$8.18** |
| Stencil | $1.53 |
| SMT joints | $0.0016 each |
| Loading fee per unique extended part | **$3.07** (base and "preferred extended" parts are free) |
| Confirm parts placement | $0.45 |
| X-ray (per leadless component) | $1.64 each at 1–10, $0.82 each at 11–50 |

**Files and sourcing**
- Required files: zipped Gerbers; **BOM** (Comment, Designator, Footprint, LCSC Part #); **CPL** (Designator, Mid X, Mid Y, Rotation, Layer; in mm).
- Out-of-stock parts can be "globally sourced" in 9–15 days.

### 4.2 Ordering steps

1. Run DRC in KiCad. Then Fabrication Toolkit exports `gerbers.zip`, `bom.csv` and `positions.csv`.
2. On jlcpcb.com, upload the Gerbers and set:
   - 4 layers, **0.8 mm**, ENIG, qty 5;
   - "Specify stackup" as needed;
   - remove the order number from the board, or specify a location for it.
3. Tick **PCB Assembly**, choose **Economic**, top side, and assemble **2** or **5** (you can assemble fewer than you fabricate).
4. Upload the BOM and CPL. Check that every line matches the right C-number and is in stock.
5. **Check every rotation in the 3D preview.** The BMD-340 pin-1, STCC4, SHT40, mic and charger footprints are the usual rotation errors.
6. Add an order remark: **"STCC4/SHT40/MEMS mic: do not wash, no ultrasonic cleaning, no conformal coating; no-clean flux."** Sensirion parts must not be washed. The mic port must not be blocked.
7. Pay, then wait for the DFM/engineer review email (usually within hours) and answer it quickly.

### 4.3 Cost estimate, delivered to Finland (est.)

The parts and fee lines are verified; PCB fabrication and shipping are estimates.

| | 2 boards | **5 boards** | 10 boards |
|---|---|---|---|
| Parts ($29.35 each) | $59 | $147 | $294 |
| Setup + stencil + 7 ext fees + confirm | $31.7 | $31.7 | $31.7 |
| X-ray (4 leadless parts per board) | ≈ $13 | ≈ $16 | ≈ $33 |
| PCB 4L 0.8 mm ENIG, 5–10 pcs (est.) | $10–30 | $10–30 | $15–35 |
| Shipping (Global Standard Direct Line … DHL, est.) | $20–35 | $20–35 | $25–40 |
| **Subtotal** | **≈ $134–169 (€120–151)** | **≈ $226–261 (€202–233)** | **≈ $390–425 (€349–380)** |
| Finnish VAT 25.5% | collected at checkout (IOSS) | at import or DDP | at import or DDP |
| Duty | ≤ €150: **€3 per HS code** + **€2 Union Handling Fee per line from 2026-11-01** | > €150: standard tariff for PCBA (rate unverified) + carrier disbursement fee (DHL FI: 2%, min €20) unless DDP | same |
| **Delivered total (est.)** | **≈ €155–195** | **≈ €255–315** | **≈ €440–500** |

Source for duties and fees: https://jlcpcb.com/help/article/eu-customs-reform-faq (updated 2026-10-26). Ordering as a private person (B2C) uses IOSS VAT when the order is ≤ €150.

**My recommendation: assemble 5.**
- About €100 more than 2 buys spares for rework, battery-life tests and two case variants.
- That said, **2 is fine if the layout is still uncertain.** A rev-B is likely anyway.

**PCBWay alternative:**
- MOQ is 1 piece.
- The search summary mentions a "$29 assembly promotion" (unverified).
- Parts come from DigiKey/Mouser, which helps if the BMD-340 sells out at JLC.
- Usually more expensive and slower for a design that is all LCSC parts.

### 4.4 Lead time (est.)

| Stage | Time |
|---|---|
| 4-layer fab | 3–5 days |
| Economic assembly | 1–3 days |
| DHL to FI | 3–6 days, or Global Standard 7–12 days |
| **Order to door** | **≈ 1.5–3 weeks** (longer if a part needs global sourcing) |

### 4.5 DFM gotchas

- Wrong rotation offsets in the CPL. Check the 3D preview.
- LGA moisture sensitivity: the BMD-340 is MSL 3, and JLC handles baking.
- Make sure the footprint's LGA pad sizes match the datasheet land pattern. Use the LCSC footprint and cross-check it against u-blox Fig. 12.
- Copper under the antenna. Check every layer, including pours.
- Solder mask between the fine STCC4 pads (NSMD pads per Sensirion).
- Pogo pads need solder mask openings and must not be paste-printed (no paste on back pads).
- If the order goes over €150, it moves out of IOSS. Expect a carrier invoice.

---

## 5. Bring-up and firmware port

### 5.1 Firmware path (recommended: keep Arduino + Adafruit nRF52 core)

The existing sketch (`agent_pendant_v0_slim_nrf52`) already uses `bluefruit.h`, `PDM.h` and `SensirionI2cStcc4`, and has PR #2's passkey pairing, CO₂ burst, connection parameters and DC/DC changes. The Seeed nRF52 core is a fork of the Adafruit core.

**Steps**

1. **Port on the nRF52840 DK first, before the boards arrive.**
   - Use the optional Adafruit 6478 breakout on the DK's I²C pins.
   - The PDM mic can be tested later. A cheap PDM mic breakout is optional.
2. **Create a custom variant**, `agent_pendant_bmd340`: copy Adafruit's `feather_nrf52840_express` variant and set:
   - `USE_LFXO`, because we fit the 32 kHz crystal. Without the crystal you would use `USE_LFRC`.
     Source: https://github.com/adafruit/Adafruit_nRF52_Arduino/blob/master/variants/feather_nrf52840_express/variant.h
   - The pin map from §3.2: `PIN_PDM_DIN/CLK`, `PIN_WIRE_SDA/SCL`, `LED_BUILTIN`, `PIN_VBAT`.
   - `PIN_PDM_PWR` is not used, because the button powers the mic. Point it at an unused pin or patch it out.
3. **Sketch changes:**
   - Remove the XIAO-only items: `VBAT_ENABLE`/P0.14, the P0.13 charge-current logic, the RGB LED.
   - New VBAT formula: 1M/1M divider, so ×2.
   - Add CHG_STAT and VBUS_DET handling. While on the charger, stop advertising-heavy work so the MCP73831 can terminate.
   - Talk starts on the button edge. The mic needs ≤ 50 ms to power up, so discard the first ~50 ms of PDM data.
   - **Keep the STCC4 in sleep between bursts.** In idle mode it draws 55 µA vs 1 µA in sleep (datasheet).
   - **Move `PAIR_PIN` to a per-device value**, e.g. derived from FICR DEVICEID, or printed on a label. Not a fixed 472913.
4. **Bootloader:**
   - Build the Adafruit nRF52 bootloader with a custom `boards/agent_pendant` config (LED and button pins).
     Source: https://github.com/adafruit/Adafruit_nRF52_Bootloader
   - Flash bootloader + SoftDevice S140 over SWD:
     - `make BOARD=… flash` (nrfjprog), or `FLASHER=pyocd`;
     - or `nrfutil device program` (nrfjprog is deprecated by Nordic);
     - or OpenOCD with the Pi probe.
   - After that, updates go over **USB through the pogo cable** (if the D+/D− pads exist: UF2/serial) or **BLE OTA DFU**, which the Adafruit bootloader supports. Or keep using SWD.
5. **Alternative later:** nRF Connect SDK/Zephyr for MCUboot, LC3/Opus and long-term support. That's a rewrite, so it isn't for v1.

**Gotchas**
- Newer nRF52840 silicon may have **APPROTECT** active. The first connection needs a "recover" (`nrfutil device recover` / `nrfjprog --recover` / OpenOCD `nrf52_recover`).
- **Recover erases the UICR, including u-blox's preprogrammed public BLE address** in CUSTOMER[0..1] (datasheet §4.3). The address is printed on the module label and can be written back. Otherwise the random static address is used, which is fine for this project.

### 5.2 Bring-up checklist (per board)

1. **Visual check** and JLC's X-ray photos.
2. **Power with no battery:**
   - Feed the BAT+ pad from a 3.7 V bench supply limited to 50 mA. Check the 3V3 rail: 3.25–3.35 V.
   - The blank-chip current is the baseline.
3. **SWD:**
   - Connect and recover. Read the FICR, then flash bootloader + SoftDevice.
   - **LFXO check:** if `Bluefruit.begin()` hangs, the 32 kHz crystal isn't starting. Check the 20 pF caps.
4. **LED blink.** Press the button and check the GPIO reads it.
5. **Mic power:** pressing the button must make Q1's drain read about 3.3 V; releasing it must give 0 V. This is the hardware privacy check.
6. **I²C scan:** the STCC4 should answer at 0x64. The SHT40 sits on the private bus and does not appear on the main bus.
   - Run `performConditioning()` (~22 s), then 3 single shots.
   - Outdoor/fresh air should read about 420 ppm; breathing on it should push it up.
7. **PDM:** record 5 s and check the RMS/level. Then stream to `tools/pendant_rx.py` over BLE.
8. **BLE pairing** with the passkey from iOS, Android and macOS (bleak).
   - Check that the connection-parameter requests are accepted: 45–60 ms + latency 30 when idle, 15–30 ms when talking.
9. **Charging:**
   - Apply 5 V to the pads. The charge current should be **1000/47 ≈ 21 mA**, and STAT should go low with the LED on.
   - Check that it terminates near 4.20 V, and that the cell and PCB stay below 40 °C.
10. **Current** (PPK2 in source-meter mode on BAT+):
    - connected idle target ≤ 30 µA;
    - talking ≈ 3–6 mA;
    - CO₂ burst ≈ 4 mA for about 1.5 s.
11. **Range:** worn under or over clothing, phone in a pocket, more than 3 m.
12. **CO₂ realism:** compare against a reference meter for 24 h, worn and on the desk, and log the drift.

### 5.3 Battery-life estimate (40 mAh, 80% usable = 32 mAh)

| Load | Current | Source |
|---|---|---|
| nRF52840 System ON, full RAM retention | 1.86 µA | BMD-340 datasheet Table 1 |
| 32 kHz crystal oscillator | 0.23 µA | BMD-340 datasheet Table 12 |
| BLE connected, ~1.35–1.8 s effective interval (45–60 ms × latency 30) | ≈ 6–15 µA | est. |
| STCC4 sleep + 3-shot burst every 5 min (≈ 1 mC per shot, from 100 µA avg at a 10 s interval) | 1 µA + ≈ 10 µA | STCC4 datasheet + est. |
| SHT40 idle | ≈ 0.4 µA | JLC listing |
| XC6206 quiescent | 1 µA | JLC/Torex listing |
| VBAT divider 1M + 1M at 4 V | 2 µA | calculation |
| MCP73831 reverse leakage, no input | ≈ 1 µA | unverified |
| **Baseline** | **≈ 23–32 µA → 0.55–0.77 mAh/day** | est. |
| Talking (CPU ~3.3 mA at 64 MHz with DC/DC + mic ~0.3–0.6 mA + radio duty) | ≈ 3–6 mA | datasheets + est. |

| Talk time per day | Battery life (est.) |
|---|---|
| 10 min/day (+0.5–1.0 mAh) | **≈ 18–30 days** |
| 30 min/day | ≈ 9–14 days |
| 60 min/day | ≈ 5–8 days |

These are only estimates; the PPK2 measurement replaces them.

---

## 6. Enclosure

**Target:** Ø28.5 × 8.5 mm disc (40 mAh cell), front slightly domed, bail at the top, antenna at the bottom.

### 6.1 Concepts

| | A: resin shell (v1) | B: jewelry hybrid (v2) | C: all-metal |
|---|---|---|---|
| Build | 2-part SLA resin, front dome + back cap. M1.2 screws or snap-fit plus a dab of glue. | Machined or cast **brass/silver bezel ring** + resin or ceramic **front face and antenna window** + metal back plate with insulated charge pads | Not possible as-is: a metal case blocks BLE. Needs a non-metal window over the antenna end. |
| Look | matte black/white/translucent; front dot pattern | jewelry | — |
| Cost/lead time | cheap, 1–2 weeks | more | — |

### 6.2 Openings and features

- **CO₂ vent:** the front **dot-pattern grille** (6–10 holes, Ø0.6–0.8 mm) over the STCC4 island. Keep the cavity behind it small and sealed off from the main cavity with a foam ring, so the sensor samples outside air, not air warmed inside the case.
  - Max's existing dot pattern from the slim CAD carries over here.
- **Mic port:** one Ø0.8–1.0 mm hole over the mic, sealed with a 0.5 mm adhesive foam gasket, kept away from the CO₂ vent.
- **Button:** centre front. Either a flexure "living-hinge" key printed into the front, or a separate key on top of the 1.5 mm TS-1187A. Travel about 0.25 mm.
- **LED:** a 0.6 mm light pipe or a thin translucent spot next to the button.
- **Charging:** 2–4 gold pads flush with the back.
  - The magnetic pogo cable snaps on; a small steel disc or magnet in the back cap provides the alignment.
  - Buy the cable first and copy its geometry.
- **Bail:** an integrated loop at the top, Ø2–3 mm hole, or a metal jump ring through a printed lug. **Keep the metal chain away from the antenna end.**
- **Skin contact:** standard SLA resins are not certified skin-safe for long wear. Wear it over a shirt, or use a nylon (MJF) or metal back plate (unverified per resin).

### 6.3 Where to make it

| Service | What | Price | Notes |
|---|---|---|---|
| JLC3DP (CN) | SLA resin 8001/9600, also MJF nylon and CNC aluminium | **resin from $0.30 per part** (help article; snippet), minimum part 5 × 5 × 5 mm | https://jlc3dp.com/help/article/photosensitive-8001-resin. Shipping and duty are the same as for JLCPCB (EU rules above). About 72 h build. |
| Craftcloud (EU marketplace) | resin, MJF, metal | quote on upload | https://craftcloud3d.com |
| Tulostan.fi (FI) | resin/FDM | "from €8.99" (snippet) | https://tulostan.fi/. Local and fast |
| Materialise OnSite (BE) | **sterling silver / brass lost-wax casting**, min wall 0.6 mm | quote on upload | https://www.materialise.com/en/industrial/3d-printing-materials/silver. For the v2 bezel. **Never over the antenna.** |

Plan for **2–3 print iterations** at about €10–40 each, delivered (est.).

---

## 7. Risks, timeline, budget, first steps

### 7.1 Risks and decision points

| # | Risk | Mitigation / decision |
|---|---|---|
| 1 | BMD-340 stock at JLC (416) sells out before ordering | Order within 2 weeks. Fallback: E73 (C356849, Ø28 board), or JLC global sourcing (+9–15 days) |
| 2 | Top-port mic stock (SPK0838: 37) | IM69D130 alternative. Put both footprints in the library, choose at order time |
| 3 | STCC4 reads body/breath/heat; spec only at 10 s sampling | Thermal island + slot + sealed vent. 120 s holdoff after talk (already in firmware). 3-shot bursts. Validate against a reference meter |
| 4 | BLE range against the body with a small ground plane | Antenna at the bottom edge, nothing metal near it. 2M PHY only when close. Test early on the DK |
| 5 | LiPo safety (tiny cell, no thermistor) | Cell with PCM, 0.5C charge, never charge while worn, Kapton insulation, 0.3 mm swell gap |
| 6 | First PCB revision has a bug | Order 5 and expect rev-B. Add test pads on every net that matters |
| 7 | Firmware port slips | Port on the DK in weeks 1–3, so the board only adds pin and power changes |
| 8 | **Decision:** Ø26 now vs Ø24 | Ø26 for v1. Ø24 with ISP1807 and/or 401015 for v2, after v1 works |
| 9 | **Decision:** USB data pads | Recommend 4 pads (VBUS, D−, D+, GND) to keep UF2/serial. Saves SWD hassle |
| 10 | Import cost over €150 | Split into ≤ €150 orders, or accept VAT + carrier fee, or choose DDP |

### 7.2 Timeline (est.)

| Week | Dates | Max | Agent (Grok Bot) |
|---|---|---|---|
| 1 | Oct 26 – Nov 1 | Order the nRF52840 DK (+ Pi probe), Electrokit cells, magnetic pogo cable; optional Adafruit 6478 + PPK2 | KiCad 10 project: schematic, LCSC footprints, ERC |
| 2 | Nov 2 – 8 | Review the layout, approve the size | Ø26 layout, DRC, 3D/STEP check; case CAD around the real STEP; BOM/CPL export. **Order JLC (Max)** at the end of the week |
| 3 | Nov 9 – 15 | DK firmware tests (BLE pairing, CO₂ via breakout) | Variant + sketch port on the DK; print case v1 (JLC3DP or local) |
| 4 | Nov 16 – 22 | Receive boards (fast shipping) | Bring-up scripts, checklist |
| 5 | Nov 23 – 29 | Bring-up, first wear test | Fixes, case v2 |
| 6 | Nov 30 – Dec 6 | Battery-life measurement, CO₂ comparison | Rev-B notes if needed |

### 7.3 Total budget (est.)

| Item | € |
|---|---|
| 5 assembled boards, delivered | 255–315 |
| nRF52840 DK + Pi Debug Probe | ≈ 77 |
| Cells (2 × 40 mAh + 1 × 55 mAh) + Electrokit shipping | ≈ 20–30 |
| Magnetic pogo cable(s), pogo pins, Kapton, misc | ≈ 15–25 |
| Case prints, 2–3 rounds | ≈ 30–100 |
| **Core total** | **≈ €400–550** |
| Optional: PPK2 (€130), Adafruit 6478 (€31 incl. VAT), J-Link EDU Mini instead of the Pi probe | + up to ≈ €160 |

Minimum path with only 2 boards and the Pi probe as the only tool: **about €170–330 (chosen; see [`MINIMUM_PATH.md`](MINIMUM_PATH.md)).**

### 7.4 What Max does first

1. **Approve the module and size:** BMD-340, Ø26 board, ≈ Ø28.5 × 8.5 mm pendant, 40 mAh 3 mm cell.
2. **Order the dev tools now** (they're needed regardless):
   - nRF52840 DK (TME FI);
   - Raspberry Pi Debug Probe (optional backup);
   - 2 × Electrokit 40 mAh + 1 × 55 mAh;
   - one magnetic 4-pin pogo cable (send me its pin pitch).
3. **Optional:** the Adafruit 6478 breakout, so CO₂ code can be tested on the DK, and a PPK2.
4. Tell me: **4 pads with USB data, or 2 charge-only pads?** And the case material for v1 (resin colour).
5. Let me build the KiCad 10 project in the Grok Bot fork. You review the 3D render and layout PDF.
6. **You** place the JLC order: Economic PCBA, 4L, 0.8 mm, ENIG, fabricate 5, assemble 5 (or 2), plus the "no wash" remark. Do this while the BMD-340 stock lasts.
7. While the boards ship, run the ported firmware on the DK and order case print v1.
8. Bring-up with the §5.2 checklist. Measure the current. Wear it for a week.

---

## 8. What I could not verify today

- **E73 height.** "3.0 mm" comes from PR #2. I didn't confirm it on the Ebyte drawing. E73 certification is also unverified.
- **IM69D130 port side.** A search summary says top port; this needs checking on the drawing.
- **Footprint pad geometry.** I didn't verify the BMD-340 antenna-zone length (my 4.5 mm assumption) or the STCC4 Design-In Guide's exact slot dimensions.
- **JLC fab details:**
  - JLC's minimum non-plated slot width;
  - whether Economic PCBA needs edge rails for a Ø26 board;
  - the exact 4L/0.8 mm/ENIG PCB price.
  All three need a live quote.
- **Import duty.** The PCBA duty rate above €150 and its HS code are unverified.
- **Search-snippet prices:** TME DK, welectron Pi probe, J-Link EDU Mini, PPK2, DigiKey/Mouser BMD-340 stock, PCBWay $29 promo, JLC3DP $0.30 resin, Tulostan.fi.
- **Estimates:** the MCP73831 reverse leakage, and all battery-life and BLE idle-current figures. A PPK2 measurement replaces them.
- **Electrokit cells.** Stock was checked earlier today (`mvp_feasibility.md`), not re-checked for this file.
- **Magnetic pogo cable** pitch, availability and price.
