# Agent Pendant: smallest buildable CO2 + push-to-talk wearable (research, 2026-10-25)

Scope: a CO2 reading every 5–10 min (single-shot, duty-cycled), hold-to-talk mic streaming compressed audio over BLE to the phone, a battery, a button, no haptics.
Current design: Ø35 × 16 mm disc (XIAO nRF52840 Sense + Adafruit STCC4/SHT41 + 402020 LiPo).
**Every enclosure size, current and battery-life figure below marked (est.) is my own estimate, not a vendor spec.**

## 1. CO2 sensors: the size gap is huge, and STCC4 is the only one that fits a pendant

| Sensor | Principle | Size (mm) | Supply | Avg current at 1 sample per 5–10 min | Single-shot? | Price / source |
|---|---|---|---|---|---|---|
| **Sensirion STCC4** (bare, LGA) | thermal conductivity | **4.0 × 3.0 × 1.2** | 2.7–5.5 V | Datasheet: <100 µA avg at a 10 s interval; 1 µA sleep; 3.7–4.2 mA peak; 500 ms per shot. **At 5–10 min: about 3–5 µA (est.)**, derived as ≤1 mC per shot ÷ 300–600 s, plus 1 µA sleep | Yes (`measure_single_shot` 0x219D, 500 ms). Sensirion recommends a 5–600 s interval for ASC | €7.82 (1 pc, excl. VAT) at SOS electronic; $10.06 at DigiKey; also stocked at Mouser |
| Sensirion SCD41 | photoacoustic NDIR | 10.1 × 10.1 × 6.5 | 2.4–5.5 V | **450 µA** at 1 shot per 5 min (3.3 V); 175 mA peak | Yes (SCD41 only) | widely stocked |
| Infineon XENSIV PAS CO2 | photoacoustic | 14 × 13.8 × 7.5 | **12 V** emitter + 3.3 V | 0.8 mA (12 V) + 6.1 mA (3.3 V) at 1 sample/min | — | — |
| Senseair Sunlight | NDIR (LED) | 34 × 21 × 12 | 3.05–5.5 V | 1–34 µA (very low power, but large) | periodic/single | — |

Sources:
- STCC4 datasheet: https://cdn.sparkfun.com/assets/2/3/8/b/e/STCC4.pdf (also https://sensirion.com/media/documents/6AED4B15/69295E41/CD_DS_STCC4_D1.pdf). Size: https://sensirion.com/products/catalog/STCC4
- STCC4 prices: https://www.soselectronic.com/en-gb/products/sensirion/stcc4-410169, https://www.digikey.com/en/products/detail/sensirion-ag/STCC4-D-R3/26769073. Stock list: https://sensirion.com/products/catalog/STCC4?show_inventory=STCC4-D-R3
- SCD41: https://sensirion.com/media/documents/E0F04247/631EF271/CD_DS_SCD40_SCD41_Datasheet_D1.pdf
- PAS CO2: https://www.infineon.com/assets/row/public/documents/24/49/infineon-pasco2v01-datasheet-en.pdf
- Sunlight: https://senseair.com/product/sunlight-co2/, https://rmtplusstoragesenseair.blob.core.windows.net/docs/Dev/publicerat/PSP12930.pdf

STCC4 caveats from the datasheet:
- It **requires an SHT4x** temperature/humidity sensor for compensation, either on its secondary I2C port or by the host writing RH/T values with `set_rht_compensation`.
- Operating range is 10–40 °C.
- ASC needs fresh air about weekly.
- After more than 3 h idle or powered off, it needs about 360 single shots, or 1 h continuous, to reach accuracy. With 5–10 min sampling the sensor is never idle that long. `perform_conditioning` helps.
- Accuracy is specified at a 10 s single-shot interval. Accuracy at 300–600 s is **not specified**, so expect somewhat noisier readings. Averaging 2–3 shots per wake is cheap (est.).
- Don't put it next to the warm MCU or charger, or against skin. It needs a vent.

**Pick: STCC4 (+ SHT40/41).** It is about 60× smaller by volume than SCD41 and about 100× lower average current at 5 min. The other sensors are physically out of the question for a pendant.

### STCC4 breakouts (no custom PCB)

| Board | Size (mm) | Includes SHT4x? | Price / source |
|---|---|---|---|
| **WAVGAT ZY-STCC4 Mini** | **15 × 11.5**, 0.4 g | **Not stated; verify before ordering.** If it doesn't, RH/T must come from elsewhere | $9.99 at https://wavgat.com/product/zy-stcc4-mini-co%e2%82%82-sensor-i%c2%b2c-breakout-module/ (China) |
| Adafruit 6478 STCC4 + SHT41 | dims not stated on the product page. Standard STEMMA QT boards are about 25.4 × 17.8 (est., which matches the earlier ~31 mm diagonal) | yes (SHT41 on the STCC4 secondary bus) | $27.50 at https://www.adafruit.com/product/6478 |
| SparkFun STCC4/SHT40 Qwiic | 1" × 1" (doc says "22.5 mm"; 1" is actually 25.4 mm) | yes | https://docs.sparkfun.com/SparkFun_CO2_Sensor_STCC4_SHT40_Qwiic/hardware/hardware%20overview |
| DFRobot Gravity STCC4 | 32 × 22 | yes (SHT40) | https://www.dfrobot.com/product-3098.html |

## 2. BLE MCU boards and modules with a mic

| Part | Size (mm) | Mic | Notes / price / source |
|---|---|---|---|
| **Seeed XIAO nRF54L15 Sense** | **21 × 17.8**, 2.5 g | onboard MSM261DGT006 PDM | BLE 6.0, LiPo charger, user button, user LED, USB-C. Measured board sleep about 1.7 µA (vs 2.4 µA for the nRF52840 XIAO). $15.99 at Seeed; €14.58 excl. VAT at Gotronic (FR). Sources: https://wiki.seeedstudio.com/xiao_nrf54l15_sense_getting_started/, http://www.gotronic.fr/download-product-page/48550, https://www.seeedstudio.com/XIAO-nRF54L15-Sense-p-6494.html, https://forum.seeedstudio.com/t/comparison-of-sleep-current-for-xiao-nrf54l15-nrf52840-and-mg24/294007 |
| Seeed XIAO nRF52840 Sense (current design) | 21 × 17.8 | onboard PDM | same footprint; sleep about 2.4 µA |
| No Logo Tech SuperNRF52840 / MINI | 18 × 21 | none | not smaller in practice. http://astrobeano.blogspot.com/2025/10/no-logo-tech-nrf52840-controller-boards.html |
| Insight SiP ISP2454 (nRF54L15 SiP, antenna inside) | **8 × 8 × 1.0** | external PDM mic needed | $11.09 at Mouser (1,757 in stock). https://www.insightsip.com/fichiers_insightsip/pdf/ble/ISP2454/isp_ble_DS2454.pdf, https://eu.mouser.com/new/insight-sip/insight-sip-isp2454-mx-ble-modules/ |
| Insight SiP ISP1807 (nRF52840 SiP) | 8 × 8 × 1.0 | external | about $12.5–12.9. https://insightsip.com/products/bluetooth-smart-modules/isp1807 |
| u-blox ANNA-B402 / B112 | 6.5 × 6.5 × 1.2 | external | **discontinued**. https://www.u-blox.com/en/product/anna-b402-open-cpu |
| Raytac MDBT50Q-P1M | 10.5 × 15.5 × 2.0 | external | https://www.raytac.com/download/index.php?index_id=43 |
| Nordic nRF54L15-QFAA (bare QFN48, 6 × 6) | 6 × 6 | external | $4.38 at DigiKey; on LCSC as C42458750, so JLCPCB can assemble it (stock not checked). https://www.digikey.com/en/products/detail/nordic-semiconductor-asa/NRF54L15-QFAA-R/24773448 |

ESP32-C3/C6 tiny boards (e.g. SuperMini, about 22.5 × 18) are no smaller than a XIAO and draw much more current during BLE, so I dropped them.

## 3. Mics and batteries

**PDM MEMS mics**, all about 1 mm thick:
- Knowles SPW0690LM4H-1: 3.10 × 2.50 × 0.85 mm, 270 µA. Source: https://cdn.thomasnet.com/ccp/30973684/351950.pdf
- Infineon IM66D130: 3.0 × 2.0 × 0.98 mm. Source: https://www.infineon.com/assets/row/public/documents/24/49/infineon-im66d130m-datasheet-en.pdf
- TDK T5838: 3.5 × 2.65 mm, 120 µA in low-power mode. $2.83 at DigiKey, **out of stock until about Dec 2026**. Sources: https://www.digikey.com/en/products/detail/tdk-invensense/MMICT5838-00-012/16903860, https://www.farnell.com/datasheets/4761948.pdf
- ICS-41351 and SPH0641LU4H-1: 3.5 × 2.65 × 0.98 mm.

The mic doesn't drive the size. Any of them works.

**Batteries:**

| Cell | Size (mm) | Capacity | Source |
|---|---|---|---|
| LiPo 301020 | 3 × 10 × 20 (about 22 long incl. protection board) | 40 mAh (60 mAh variant exists) | https://www.fpbattery.com/wp-content/uploads/2024/06/fpbattery-301020-3.7V-40mAh-Lithium-Polymer-Battery-Specification.pdf |
| LiPo 401020 | 4 × 10 × 20 | 55 mAh | https://thorbattery.com/products/lipo-battery-models/ |
| LIR1254 coin | Ø12 × 5.4 | 60–65 mAh | https://www.lideapower.com/product_details/14.html |
| LIR2032 coin | Ø20 × 3.2 | 35–40 mAh | https://www.gmbattery.com/dl/cp11/li-ion/Button/LIR2032.pdf |
| 402020 (current design) | 4 × 20 × 20 | 120 mAh | — |

**Power budget (est.):**

| Load | Current | Per day |
|---|---|---|
| BLE connection to phone held at a long interval | 10–30 µA | 0.24–0.72 mAh |
| Board sleep (XIAO) | about 2 µA | 0.05 mAh |
| STCC4 + SHT4x at 5 min | about 5 µA | 0.12 mAh |
| Talking 10 min/day (PDM + ADPCM/Opus + BLE TX) | 5–12 mA | 0.8–2.0 mAh |
| **Total** | | **about 1.2–2.9 mAh/day** |

- **About 1 week needs roughly 10–20 mAh usable**, so a 40 mAh cell gives margin.
- A 301020 at 40 mAh gives about 2–4 weeks nominal, or about 1.5–3 weeks after derating (est.).
- If Opus encoding on the MCU proves heavy, use ADPCM, or Opus on the nRF54L15's faster core (est.).

## 4. Market proof

| Product | Size | Notes / source |
|---|---|---|
| **OpenCO2 Mini** (STCC4 + SHT40 + ESP32-S3) | **20 × 15 mm** keychain (Hackster reports 20 × 27) | USB-C powered, **no battery**. Proves STCC4 + MCU fits in about 20 × 15. https://www.tindie.com/products/davidkreidler/openco2-mini/, https://www.hackster.io/news/david-kreidler-s-openco2-mini-sensor-delivers-carbon-dioxide-monitoring-on-the-go-a33a70cdecd9 |
| AirSpot wearable CO2 | 36 × 48 × 12 mm, 17 g | https://seetheair.org/2026/01/22/airspot-review-the-wearable-co2-monitor-that-delivers-real-time-clarity/ |
| Limitless Pendant | about 31.9 mm wide × 16 mm thick | https://indianexpress.com/article/technology/artificial-intelligence/what-is-limitless-pendant-9275386/ |
| Plaud NotePin | 51 × 21 × 11 mm, 16.6 g, 270 mAh | https://eu.plaud.ai/pages/hardware-comparison |
| Bee Pioneer | about 46 × 20 mm (converted from 1.8 × 0.8 in) | https://www.forbes.com/sites/forbes-personal-shopper/article/best-ai-wearables/ |

## 5. Answers

### (A) Smallest DIY build from off-the-shelf boards, no custom PCB

**Picks:**
- **MCU: XIAO nRF54L15 Sense.** It has an onboard PDM mic, LiPo charger, **onboard user button usable as push-to-talk** through a flexible case membrane, onboard LED, and the lowest measured sleep current of the XIAOs.
- **CO2: ZY-STCC4 Mini (15 × 11.5).** If it lacks an SHT4x, either use the Adafruit 6478 (bigger, adds about 5–8 mm to the footprint) or wire a tiny SHT40 breakout to the STCC4 secondary bus.
- **Battery: LiPo 301020, 40 mAh.**

**Stack** (all est.):
- ZY-STCC4, about 2.8 mm, behind a vent
- XIAO, about 3.5–4 mm incl. USB-C
- 301020 battery, 3 mm
- tape and wires, about 1 mm
- 1.2 mm walls on each side

**Outer size: about 25 × 21 × 12–13 mm rounded rectangle, or about Ø30 × 13 mm round (est.).** That is roughly 40–55% less volume than the Ø35 × 16 version. Parts changes vs the current design: XIAO nRF54L15 Sense instead of the nRF52840 Sense, ZY-STCC4 instead of the Adafruit board, 301020 instead of 402020, and the separate button is optional. Firmware must move to Zephyr/nRF Connect SDK (the nRF54L15 has no mature Arduino core; est./verify).

**Battery life: about 1.5–3 weeks at 10 min/day of talking (est.).**

### (B) Smallest with ONE custom PCB (4-layer, JLCPCB assembly)

One round 4-layer board, about Ø18–20 mm, components on both sides.

| Part | Choice | Size (mm) | Price | Source |
|---|---|---|---|---|
| MCU + radio | **nRF54L15-QFAA** (chip down, LCSC C42458750) + chip antenna; **or** ISP2454 SiP (antenna inside, easier RF, may need consignment to JLC) | 6 × 6 / 8 × 8 | $4.38 / $11.09 | DigiKey / Mouser links above |
| CO2 | **STCC4-D-R3** | 4 × 3 × 1.2 | €7.82 | SOS / DigiKey / Mouser |
| RH/T | SHT40-AD1B | 1.5 × 1.5 | $1.94 | https://www.digikey.com/en/products/detail/sensirion-ag/SHT40-AD1B-R2/13532084 |
| Mic | IM66D130 or SPW0690 (T5838 out of stock) | ~3 × 2 | ~$1–3 (est.) | above |
| Charger + buck | 1-cell linear charger + tiny buck (part not chosen yet) | — | ~$1–2 (est.) | — |
| Other | side tact switch, LED, **pogo charge pads instead of USB-C** (saves about 2 mm of thickness) | — | — | — |
| Battery | LiPo 301020, 40 mAh | 3 × 10 × 20 | a few € (est.) | AliExpress |
| PCB + assembly | 5 boards | — | roughly €30–80 incl. setup (**est., get a JLCPCB quote**) | jlcpcb.com |

**Outer size: about Ø24 × 8–9 mm (est.)**, built up as:
- 1 mm shell
- about 3.4 mm PCB with parts
- 3 mm cell
- 0.5 mm gap
- 1 mm shell

The 20 mm cell length sets the minimum diameter. With an LIR1254 instead, it becomes about Ø20 × 10–11 mm (est.). Battery life is about the same as (A), around 1.5–3 weeks (est.). Lead time is about 2–3 weeks for design, plus about 1–2 weeks for JLC fab and shipping (est.).

**Physical floor today:** about 20–24 mm across and 8–10 mm thick. Below that, the battery, not the electronics, sets the size.
