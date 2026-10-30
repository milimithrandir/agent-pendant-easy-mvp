# Agent Pendant: hand-built MVP feasibility and where to buy (checked 2026-10-26, Helsinki)

Scope: hand-built, no custom PCB. Hold-to-talk mic streamed over BLE to the phone, CO2 reading every 5–10 min, tiny LiPo, 3D-printed case. Max orders the parts himself; nothing has been ordered.

- **Prices** were read live from the store page today unless marked *(search snippet)* or *(not re-verified)*.
- **(est.)** marks my own estimates. They are not vendor specs.
- **VAT:** Finland's VAT is 25.5%. EU shops charge Finnish VAT to Finnish consumers at checkout (OSS), so a price shown incl. DE/FR/SE VAT will shift slightly.
- **Duty on non-EU parcels:** since 1 Jul 2026, Finland charges €3 customs duty per goods item (per commodity code, not per unit) on parcels from outside the EU worth up to €150. Finnish VAT is charged on top. Sources: https://tulli.fi/en/customs-fee-upcoming and the EU Commission notice from 2026-06-29. Seeed's product page says its shipping fee already includes the €3 duty. The UK (The Pi Hut, Pimoroni UK) also counts as non-EU.

---

## 1. Key findings (read these first)

1. **The nRF54L15 Sense gives no size gain over the nRF52840 Sense in a hand-built device.**
   - Both boards measure 21 × 17.5–17.8 mm, and both have an onboard PDM mic, charger, USB-C and LED. The outer size is set by the CO2 board and the battery, not by the XIAO.
   - The nRF54L15 does have real advantages: an onboard user button, lower measured sleep current (about 1.7 µA vs 2.4 µA, a Seeed forum measurement) and BLE 6.0.
2. **The nRF54L15 Sense charges its battery at about 200 mA.** Seeed confirmed this in the wiki discussion at https://github.com/Seeed-Studio/wiki-documents/discussions/2803: an SGM40567 charger with R4 = 120 kΩ.
   - On a 40 mAh cell that is **5C**. The Electrokit 40 mAh cell is rated 1C max (40 mA). **Do not plug a 40 mAh cell into the nRF54L15 XIAO's battery pads and charge it over USB.**
   - Fixes:
     - swap the 0402 charge-set resistor (hot-air rework; the value has to be checked against the SGM40567 datasheet, e.g. roughly 600 kΩ to 1.2 MΩ for 40–20 mA if I = 24000/R holds);
     - charge the cell off-board;
     - or use a cell of 200 mAh or more, which is too big for this device.
3. **The XIAO nRF52840 Sense charges at 50 mA by default** (BQ25101; P0.13 open or HIGH gives 50 mA, LOW gives 100 mA).
   - That fits the **Electrokit 55 mAh, 15 × 10 × 5 mm** cell, which is rated 1C = 55 mA. Leave P0.13 alone, and never drive it LOW in firmware.
4. **The ZY-STCC4 Mini (WAVGAT) product page does not mention an SHT4x.**
   - It states only the STCC4, 2.7–5.5 V, I2C, 15 × 11.5 mm and 0.4 g. Treat it as **having no humidity sensor** until the seller confirms otherwise.
   - The I2C address isn't stated. The STCC4 default is 0x64 (0x65 alternative), per Sensirion and Adafruit.
   - The page is behind a captcha for scripts, so I couldn't inspect the gallery photos.
   - Without an SHT4x the STCC4 runs uncompensated, or the host has to feed it RH/T through `setRhtCompensation`. Sensirion's datasheet says it *requires* an SHT4x for its specified accuracy.
5. **Adafruit 6478 (STCC4 + SHT41) is in stock in the EU at Kiwi:** €24.49 excl. VAT, 7 pcs, ships today. It's a known-good path, but the board is bigger; Adafruit doesn't publish its size, and I estimate about 25 × 18 mm.

---

## 2. Parts: where to buy

### 2a. EU / Finland

| Part | Shop and URL | Price | Stock (today) | Delivery to FI |
|---|---|---|---|---|
| **XIAO nRF54L15 Sense** (101991422, unsoldered) | Gotronic (FR) https://www.gotronic.fr/cat-cartes-xiao-nrf54l15.htm (code 39312) | €17.50 incl. FR VAT / €14.58 excl. | In stock | Colissimo Intl. Price shown at checkout; about €12.55+ for 500 g *(search snippet)*. About 3–6 business days (est.) |
| XIAO nRF54L15 Sense (pre-soldered headers, 114993697) | BerryBase (DE) https://www.berrybase.de/en/detail/019e4fd12868713abe3351d38d50e657 | €19.50 incl. DE VAT | 52 pcs (the "ohne Header" variant is unavailable) | DHL €9.90 to FI *(shipping-terms page via search)*. Dispatch 1–3 days, about 3–5 business days total (est.) |
| XIAO nRF54L15 Sense | Botland https://botland.store/xiao/27509-... | €17.10 *(search snippet)* | Back order. The page says "sells to registered businesses only" (VAT ID) | GLS EU from €8.07 |
| XIAO nRF54L15 Sense | Kiwi, Partco | — | Not listed (Partco lists only the nRF54LM20A Sense, at €33.40) | — |
| XIAO nRF54L15 Sense | Mouser | $15.90 | 0 in stock, per a third-party mirror. Mouser blocks scripts, so I couldn't check directly | — |
| **XIAO nRF52840 Sense** (102010469), recommended | BerryBase https://www.berrybase.de/en/seeed-studio-xiao-nrf52840-sense-bluetooth-5.4-nfc-6-achsen-imu-mikrofon-imu-20-gpios-3-3v/auswahl-ohne-header | €19.90 incl. DE VAT, no headers | 94 pcs | €9.90, about 3–5 business days (est.) |
| XIAO nRF52840 Sense | Kiwi https://www.kiwi-electronics.com/en/seeed-studio-xiao-nrf52840-sense-11213 | €14.99 excl. VAT | **Out of stock, expected 06-11-2026** | PostNL from €5.94 / €10.10 *(Kiwi help page via search)* |
| XIAO nRF52840 Sense | Partco https://partco.fi/tuote/seeed-xiao-ble-nrf52840-sense-303 | €33.40 incl. 25.5% VAT | "Ei varastossa" (not in stock), availability unknown | — |
| **Adafruit 6478 STCC4 + SHT41** | Kiwi https://www.kiwi-electronics.com/en/adafruit-stcc4-and-sht41-co2-temperature-amp-humidity-sensor-stemma-qt-qwiic-20617 | €24.49 excl. VAT (about €30.7 incl. 25.5%, est.) | 7 pcs. "Shipped on Monday when ordered now" | Kiwi ships same day if ordered before 17:00 CET; about 2–5 business days (est.) |
| Adafruit 6478 | The Pi Hut (UK, so non-EU and dutiable) https://thepihut.com/products/adafruit-stcc4-and-sht41-co2-temperature-humidity-sensor | £26.40 | Sold out | — |
| Bare STCC4-D-R3 (4 × 3 × 1.2 mm LGA) | SOS electronic https://www.soselectronic.com/en-gb/products/sensirion/stcc4-410169 | €7.82 excl. VAT *(yesterday's research; the page returned 403 today)* | Not re-verified | — |
| Bare STCC4-D-R3 | DigiKey https://www.digikey.com/en/products/detail/sensirion-ag/STCC4-D-R3/26769073 | $10.06 | 131 pcs *(search snippet; DigiKey blocks scripts)* | — |
| **LiPo 55 mAh, 15 × 10 × 5 mm, PCM, JST-PH**, recommended with the nRF52840 | Electrokit (SE) https://www.electrokit.com/en/batteri-lipo-3.7v-55mah | 55 SEK (displayed incl. VAT) | 146 pcs. "Leveranstid ca 1–3 vardagar" (delivery about 1–3 working days) | Ships to FI; cost shown at checkout (not verified) |
| LiPo 40 mAh, 20.5 × 10 × 3 mm, PCM, BEC plug | Electrokit https://www.electrokit.com/en/battery-lipo-3.7v-40mah-bec | 55 SEK | 18 pcs | Same as above. **Max charge 40 mA**, too low for either XIAO's default charger |
| LiPo 100 mAh LP-401230 (4 × 12 × 30 mm) | BerryBase (on the nRF52840 page) | €3.40 | 100+ | Too long for the small build; bench use only |
| Mini slide switch SPDT, 2.54 mm | Kiwi https://www.kiwi-electronics.com/en/mini-slide-switch-spdt-3866 | €0.99 excl. VAT | 150 pcs | With the Kiwi order |
| Slide switch ON-ON 8.6 × 4.3 × 4 mm | BerryBase (add-on on the product page; SKU SIS-86434-G) | €0.70 | 88 pcs | With the BerryBase order |
| 6 × 6 × 5 mm tact switch (needed for the nRF52840, which has no user button) | Electrokit (art. 4100 1412, shown on the LiPo page) | 1.25 SEK | 4787 pcs | With the Electrokit order |
| Optional low-current external charger | Kiwi Adafruit Micro-Lipo USB-C (ADA-4410) https://www.kiwi-electronics.com/en/adafruit-micro-lipo-lipo-li-ion-charger-with-usb-c-20852 | €5.49 excl. VAT | 19 pcs | 100 mA default, which is still above 1C for 40/55 mAh cells. Not a fix as-is |
| Case print | Max's own printer; Tulostan.fi https://tulostan.fi/ ("from €8.99" *(search snippet)*); 3Dtulostuspalvelu.fi (from €20) | — | — | Local. Resin (SLA) is best for walls of 1–1.2 mm |

### 2b. China / non-EU (each parcel: €3 per goods category + 25.5% VAT)

| Part | Shop and URL | Price | Stock | Delivery / notes |
|---|---|---|---|---|
| XIAO nRF54L15 Sense | Seeed direct https://www.seeedstudio.com/XIAO-nRF54L15-Sense-p-6494.html | $15.99 ($13.99 at 10+) | In stock | Shipping shown at checkout. Seeed says the fee includes the €3 EU duty. Typically 1–2+ weeks from CN (est.). A Seeed DE warehouse exists, but I couldn't confirm it stocks this SKU |
| XIAO nRF54L15 Sense | AliExpress https://www.aliexpress.com/i/3256810571841633.html (pre-soldered) | About $10.70 unsoldered / $12.13 pre-soldered + about $8.63 shipping *(Oct 2025 launch article; AliExpress blocks this box, so not live)* | Not verified | About 1–3 weeks (est.) |
| XIAO nRF52840 Sense | Seeed direct https://www.seeedstudio.com/Seeed-XIAO-BLE-Sense-nRF52840-p-5253.html | $15.90 ($13.90 at 10+) | In stock | As above |
| XIAO nRF52840 Sense | AliExpress | €20.28 *(earlier session, not re-verified)* | — | — |
| **ZY-STCC4 Mini** (15 × 11.5 mm, 0.4 g, SKU WAV751) | WAVGAT https://wavgat.com/product/zy-stcc4-mini-co%e2%82%82-sensor-i%c2%b2c-breakout-module/ | $9.99 (was $10.89) | Not stated | Shipping to FI not stated, so ask the seller. **SHT4x not mentioned.** I found no AliExpress listing |
| DFRobot Gravity STCC4 + SHT40 (32 × 22 mm, too big) | DFRobot https://www.dfrobot.com/product-3098.html, also AliExpress https://www.aliexpress.com/i/3256811912945615.html | $14.90 | Not verified | Bench fallback only |
| SHT40 tiny breakout (19 × 7 × 2.81 mm, 0x44, 1.27 mm pitch) | 7Semi (India, non-EU) https://7semi.com/7semi-sht40-temperature-humidity-sensor-breakout-1-27mm-pitch/ | $2.60 | 80 in stock, "ships within 24 h" | Shipping cost not checked. This is the add-on that would let the ZY-STCC4 work properly |
| LiPo 301020 40 mAh | AliExpress (many listings) / Amazon.com AKZYTUE | About $1.3–2.1 at supplier level; $7.49 at Amazon *(search snippets)* | Not verified live | About 1–3 weeks (est.). Check charge rating, connector and protection board |
| Slide switch / 6 mm tact | AliExpress / LCSC | Cents *(not verified)* | — | Not worth a separate €3-duty parcel |
| Case | JLC3DP https://jlc3dp.com/3d-printing-quote | Resin from $0.30 per part + shipping by quote *(search snippet)* | — | Shipping to FI needs an instant quote (STL upload). About 1–2 weeks (est.) |
| Bare STCC4 | LCSC | Not verified (search found no LCSC listing) | — | — |

---

## 3. Recommended carts

### Fastest: a working prototype this week or next. All EU, nRF52840 Sense, about €75–95 incl. shipping (est.)

1. **BerryBase:** XIAO nRF52840 Sense, no headers (€19.90) and slide switch 8.6 × 4.3 × 4 mm (€0.70). Shipping €9.90.
2. **Kiwi:** Adafruit 6478 STCC4+SHT41 (€24.49 excl. VAT) and mini slide switch spare (€0.99). PostNL about €6–10.
3. **Electrokit:** 55 mAh LiPo 15 × 10 × 5 (55 SEK), 2× 6 × 6 tact (2.5 SEK) and optionally the 40 mAh cell for later (55 SEK). Shipping shown at checkout.
4. **Case:** print it yourself, or Tulostan.fi.

ETA: about 2–5 business days per shop (est.). Firmware for this board already exists in `firmware/agent_pendant_v0_slim_nrf52`; it just needs the new battery and pins.

### Cheapest: about €45–60 delivered (est.), 1–3 weeks

1. **Seeed direct:** XIAO nRF52840 Sense ($15.90), and optionally the nRF54L15 Sense ($15.99) in the same parcel for v0.2. Shipping includes the €3 duty.
2. **WAVGAT:** ZY-STCC4 Mini ($9.99). Ask about the SHT4x and the shipping cost first.
3. **7Semi:** SHT40 tiny ($2.60), only if the ZY board has no SHT4x.
4. **Electrokit:** 55 mAh LiPo and tact switches. A Chinese 301020 is cheaper but doesn't fit the 50 mA charger.

Each non-EU parcel costs €3 per goods category plus VAT. Several tiny China parcels can cost more than one EU order.

**Best size path (est. about 25 × 21 × 12 mm):** nRF52840 Sense + ZY-STCC4 (+ SHT40) + 55 mAh cell. Order the Kiwi Adafruit 6478 too, for bench bring-up while the ZY board ships from China.

---

## 4. Feasibility for Max

### Soldering
- **XIAO boards:** 2.54 mm castellated edge pads, plus BAT+/BAT− pads on the back. Easy to moderate with a fine tip.
- **nRF54L15 back pads:** the SWD and NFC pads are small, but this build doesn't need them.
- **Adafruit 6478:** 0.1" holes. Easy.
- **ZY-STCC4:** the pad pitch isn't stated on the page; verify on arrival.
- **Tact and slide switches:** through-hole or 2 mm pitch. Easy.
- **Bare STCC4 LGA:** not hand-solderable without hot air and a stencil. Skip it for the MVP.
- **Wire:** 30 AWG silicone or kynar. Keep the I2C leads under about 5 cm.

### CO2 sensor and library
- **Library:** Sensirion Arduino library `arduino-i2c-stcc4`, v1.1.0 (2026-04-30). It has `measureSingleShot()`, `enterSleepMode()`/`exitSleepMode()`, `setRhtCompensation()` and `performConditioning()`. Default address 0x64; it depends on Sensirion Core.
- **Single-shot timing:** single-shot every 5–10 min is supported. Accuracy at that interval isn't specified (datasheet), so average 2–3 shots per wake (est.).
- **Operating range:** **10–40 °C**. Outdoors in a Finnish winter is out of spec. Keep it under clothing or accept bad readings outdoors.
- **Placement:** needs a vent, and keep it away from the battery, charger and skin.

### Firmware support today

**nRF52840 Sense**
- Fully supported: Seeed Arduino core and Adafruit Bluefruit + `PDM.h`.
- Our sketch already does hold-to-talk, IMA-ADPCM at 16 kHz over GATT notify (about 64 kbit/s), the privacy switch, CO2 alerts and battery voltage.
- Changes needed:
  - CO2 timing to 5–10 min single shot;
  - `setRhtCompensation` if there's no SHT;
  - never set P0.13 LOW.
- Estimated effort: 1 evening.

**nRF54L15 Sense**
- **Official:** Seeed supports nRF Connect SDK/Zephyr and PlatformIO. In mid-2025 Seeed staff said there were no plans for Arduino; Japanese reseller Marutsu still states "本製品はArduinoをサポートしていません" (this product does not support Arduino).
  - The Zephyr DMIC sample captures 16 kHz mono.
  - Community project Toastee0/Seeed-Xiao-nRF54L15 shows `bap_dmic`: PDM → LC3 → **LE Audio broadcast**. That isn't the GATT stream our phone app expects, and phone support for LE Audio broadcast reception is limited.
- **Arduino:** there's a community core, lolren/nrf54-arduino-core, v1.0.22 from 2026-10-18 with 48 stars. Seeed's own wiki now points to it: https://wiki.seeedstudio.com/xiao_nrf54l15_sense_arduino/.
  - BLE peripheral/GATT/notify is "Implemented" (single link).
  - It has a `Bluefruit52Lib` compatibility layer ("Partial").
  - PDM is "Partial": it uses its own `Pdm` class with `startStream`/`queueStreamBuffer`. There is **no `PDM.h` drop-in**.
  - Timed System OFF and System ON idle are implemented. There's a mic/IMU rail switch (`BoardControl::setImuMicEnabled`).
  - No BLE DFU.
  - **I found no public demo of continuous PDM → BLE GATT audio streaming on the nRF54L15.**
- **Porting our sketch** (est. 1–3 weeks hobbyist time):
  - Bluefruit calls: moderate (compatibility layer).
  - PDM: rewrite the capture loop.
  - FreeRTOS tickless delay: replace.
  - Pins and LED: remap. The onboard user button P0.00 can serve as push-to-talk.
- **Hardware issues:** charging current (200 mA) and an RF-switch pin on GPIO P2.05. Per a user report in Seeed's discussion, that pin can't be automatically gated, and the switch was reported to draw about 100 µA. If true, that's roughly 2.4 mAh/day, about doubling the budget on a 40 mAh cell (est.).

**Verdict on boards:** use the **nRF52840 Sense for MVP v0.1**. Move to the nRF54L15 later, or skip straight to the custom PCB.

### Battery and charging
- Use only cells with a protection board (PCM).
- Match the cell to the charger: nRF52840 at 50 mA with a cell of 50 mAh or more, i.e. the 55 mAh Electrokit cell.
- Never charge while worn under clothing on the first builds. Leave 0.5 mm of swelling room in the case. Insulate the pads with Kapton.
- If the nRF54L15 is used, swap its charge resistor or charge the cell off-board.

### Realistic outer size, hand-built with wires (est.)

| Build | Stack (thickness) | Outer size |
|---|---|---|
| nRF52840 Sense + ZY-STCC4 + 55 mAh | XIAO + USB-C about 3.5 mm, then ZY (about 2 mm) beside the battery (5 mm), under the XIAO footprint (11.5 + 10 = 21.5 mm wide), wires about 1 mm, walls 2 × 1.2 mm | **about 25 × 21 × 12 mm, or about Ø29 × 12 mm round (est.)** |
| nRF52840 Sense + Adafruit 6478 + 55 mAh | 6478 about 25 × 18 mm plus STEMMA connectors, stacked | **about 28 × 22 × 15 mm, or about Ø32–34 × 15 mm (est.)**, barely smaller than the current Ø35 × 16 |
| nRF54L15 Sense + ZY-STCC4 + 40 mAh 301020 | Same footprint, 2 mm thinner cell | about 25 × 21 × 10–11 mm (est.), only after the charge-current fix |

### Time to first working prototype (competent hobbyist, est.)

**nRF52840 path: about 1–2 weeks.**
- Parts: 2–5 business days (EU).
- Wiring and bench test: 1 evening.
- Firmware tweaks: 1 evening.
- Case CAD + 2–3 print iterations: 2–4 evenings.
- With the ZY-STCC4 from China, add 1–3 weeks for that part.

**nRF54L15 path:** add 1–3 weeks for the firmware port and the charger fix.

---

## 5. Top 5 risks
1. **LiPo charge current vs. tiny cell.** nRF54L15: 200 mA, i.e. 5C on 40 mAh. nRF52840: 50 mA is fine only with a cell of 50 mAh or more. This is a safety issue: swelling or fire against the body.
2. **ZY-STCC4 unknowns:** no SHT4x stated, so degraded CO2 accuracy; unknown pinout; single seller, with shipping and lead time unknown. Mitigation: add the 7Semi SHT40 or ask WAVGAT; bench with the Adafruit 6478.
3. **nRF54L15 software maturity.** A community Arduino core ("Partial" PDM, Bluefruit compatibility), no public GATT-audio demo, the RF-switch current report, and flashing/bootloader issues people reported in Seeed's thread (the SAMD11 bridge).
4. **CO2 data quality.** Accuracy at 5–10 min intervals is unspecified, the range is 10–40 °C, and body or MCU heat interferes. It needs a vent and weekly fresh air for ASC.
5. **Mechanical/RF.** Hand wiring in a 12 mm case, strain on the wires, USB-C access, the button through the case, and BLE antenna detuning against the body. The audio stream (about 64 kbit/s ADPCM) needs a short connection interval and the phone app to keep up.

## 6. Verdict
- **GO for a hand-built MVP**, using the **XIAO nRF52840 Sense** (supported, in stock at BerryBase, firmware already written) + the **Adafruit 6478** for immediate bring-up + the **Electrokit 55 mAh** cell.
- Order the **ZY-STCC4 (+ SHT40)** in parallel for the about 25 × 21 × 12 mm build (est.).
- **NO-GO for the nRF54L15 as the first MVP board.** It isn't smaller in a hand-built device, and its charger is unsafe with 40–55 mAh cells unless reworked.
- **Fallbacks:**
  - (a) if the ZY board lacks an SHT and the SHT40 is hard to get, ship v0.1 with the Adafruit 6478 at about Ø32–34 × 15 mm (est.);
  - (b) if the size is still too big, move to the custom PCB (about Ø24 × 8–9 mm, est.) from research_smallest.md;
  - (c) revisit the nRF54L15 for v0.2 once the charge resistor is swapped and a GATT PDM stream is proven on the bench.
