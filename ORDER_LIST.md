# Easy MVP: what to order

**Re-checked live on 2026-10-30, about 15:30–15:45 Europe/Helsinki (EET).** Max orders everything himself. Nothing has been ordered by Grok Bot.

Labels: **live** = read from the shop page today · **Oct 27** = from the 2026-10-27 research, not re-readable today · **unverified** = not confirmed anywhere · **est.** = estimate.

## TL;DR

| Shop | What | Subtotal (delivered to Finland) |
|---|---|---|
| **BerryBase (DE, EU, no duty)** | XIAO nRF52840 Sense + 100 mAh LiPo + 2 buttons | **≈ €33.46** (up to ≈ €34.80 if re-priced to Finnish VAT) |
| **AliExpress (CN)** | 1 × STCC4 + SHT40 CO2 module | **≈ €15.79–18.79** (incl. €3 duty) |
| Case | free 3D print at Espoo library makerspace (Sello / Iso Omena) | €0–3 |
| **Total** | | **≈ €49–56** (same range as before) |

**Order first:** the BerryBase order (it is in stock and arrives in about 3–5 business days, est.). Order the AliExpress module the same day, because it takes 1–3 weeks (est.). You can bring up the board, BLE and hold-to-talk audio before the CO2 module arrives.

## What changed since the 2026-10-27 list
- **LP-402025 150 mAh cell: now SOLD OUT at BerryBase** (live, "Currently not available"). Replaced with the **LP-401230 100 mAh** cell (in stock). It is 4 × 12 × 30 mm instead of 4 × 20 × 25 mm, so the bare stack becomes about 30 mm long (the cell sticks out past the 21 mm board), and battery life drops to about 2–5 days (est.).
- **6 × 6 mm H 5.0 mm button: only 1 left** (live). Replaced with the **H 4.3 mm** version (100+ in stock, €0.13). It is lower, which suits the case better.
- XIAO nRF52840 Sense: unchanged at €19.90, 84 in stock (was 85).
- AliExpress STCC4 module: **the listing still exists** (title read live), but **the price could not be re-read today** (AliExpress blocks automated reads). €12.79 is from Oct 27. Check it in the app before you pay.
- Kiwi Adafruit 6478 (fallback): unchanged at €24.49 ex VAT, **5 in stock** (was 7).
- Correction: the STCC4 is a thermal-conductivity CO2 sensor, not NDIR. It measures real CO2 (not a VOC guess), with ±(100 ppm + 10 %) accuracy.

---

## Order 1: BerryBase (Germany, EU, no customs)

| Qty | Item | Unit price | Link | Status (2026-10-30) |
|---|---|---|---|---|
| 1 | Seeed XIAO nRF52840 **Sense**, **"Ohne Header" (without headers)** | €19.90 | https://www.berrybase.de/en/seeed-studio-xiao-nrf52840-sense-bluetooth-5.4-nfc-6-achsen-imu-mikrofon-imu-20-gpios-3-3v/auswahl-ohne-header | **live**, 84 in stock, ships in 1–3 days |
| 1 | PKCell LP-401230 LiPo 3.7 V **100 mAh**, 2-pin JST, 4 × 12 × 30 mm | €3.40 | https://www.berrybase.de/en/lp-401230-lithium-polymer-lipo-battery-3.7v-100mah-with-2-pin-jst-connector | **live**, 88 in stock |
| 2 | Short-stroke pushbutton 6 × 6 mm, **H 4.3 mm** (1 spare) | €0.13 | https://www.berrybase.de/en/short-stroke-pushbutton-vertical-pcb-mounting-6x6mm-h-4.3mm | **live**, 100+ in stock |
| | DHL Parcel to EU countries (orders €0.01–199.99) | €9.90 | https://www.berrybase.de/en/footer/information/payment-dispatch/ | **live** |
| | **Subtotal** | **€33.46** | | |

Notes
- Make sure the variant says **Sense** (it has the microphone) and **without headers** (thinner).
- Prices include German VAT. A Finnish delivery may be re-priced to 25.5 % VAT at checkout (about +€1.30, **unverified**).
- **Unverified: whether BerryBase will ship the LiPo to Finland.** DHL restricts lithium cells in international parcels, and BerryBase's shipping page doesn't say. Put the cell in the cart and check the checkout. If it is refused, see the fallbacks below.
- Optional, if you want to save a step later: buy a 2nd LP-401230 (+€3.40) as a spare.
- Want the smaller 150 mAh cell instead? Use "Notify me" on the [LP-402025 page](https://www.berrybase.de/en/lp-402025-lithium-polymer-lipo-battery-3.7v-150mah-with-2-pin-jst-connector) (€3.30 when in stock), or see the AliExpress fallback.

## Order 2: AliExpress (one item type = one €3 duty)

| Qty | Item | Price | Link | Status |
|---|---|---|---|---|
| 1 | STCC4 CO2 sensor module **with onboard SHT40** (3-in-1 CO2 / temp / humidity, I2C) | ≈ €12.79 | https://www.aliexpress.com/item/1005012973112114.html | listing exists (**live**); price **Oct 27**; new listing, 0 sales on Oct 27 |
| | Finnish customs duty, collected at checkout | €3.00 | | rule from Oct 27 research (Tulli/Yle summary) |
| | Shipping | €0–3 | | **unverified** |
| | **Subtotal** | **≈ €15.79–18.79** | | |

Notes
- Buying 2 (one spare) is still one item type, so still one €3 duty: about +€12.79.
- Choose a variant **without** pin headers soldered on, or with loose headers (thinner).
- Risk: new seller, no sales history, and we don't know how the SHT40 is wired. See [`BUILD_GUIDE.md` §6](BUILD_GUIDE.md#6-test-the-co2-sensor) for the 2-minute check when it arrives.
- AliExpress delivery to Finland: about 1–3 weeks (est.).

## Free
- **Case:** Espoo City Library makerspaces at **Sello** or **Iso Omena** have free 3D printers (reserve via Varaamo). Grok Bot can make the STL once the parts arrive and are measured.
- Or, for the first week: a clear badge pouch / clear heat-shrink with a window cut over the CO2 sensor and the mic.

---

## Fallbacks (only if something above fails)

| Problem | Fallback | Price | Link | Status |
|---|---|---|---|---|
| AliExpress STCC4 module is a dud, or you don't want to wait | **Adafruit 6478 STCC4 + SHT41** (the board the firmware was written for, zero code changes) from Kiwi Electronics (NL, EU) | €24.49 ex VAT ≈ **€30.74 incl. 25.5 % VAT** + PostNL ≈ €6–10 | https://www.kiwi-electronics.com/en/adafruit-stcc4-and-sht41-co2-temperature-amp-humidity-sensor-stemma-qt-qwiic-20617 | **live**, 5 in stock; shipping **unverified** (snippet) |
| | Total with this instead of AliExpress | | | **≈ €70–76** |
| Another STCC4 board | DFRobot Gravity STCC4 (SEN0678), onboard SHT40, 32 × 22 mm (bigger) | $14.90 at DFRobot | https://www.dfrobot.com/product-3098.html | price from search snippet, stock **unverified**; import VAT/duty extra |
| STCC4 not available at all | SCD41 module (photoacoustic, more accurate, thicker, needs ≈ 1–2 h firmware change) | ≈ €11.69–15.40 | e.g. https://www.aliexpress.com/item/1005007925654431.html | **Oct 27** Ali card |
| BerryBase won't ship the LiPo to Finland | 402030 200 mAh LiPo on AliExpress (+€3 duty as 2nd item type) | ≈ €5.33 | https://www.aliexpress.com/item/1005010776246310.html | **Oct 27** Ali card |
| | or a Finnish/Nordic shop that ships LiPos domestically (e.g. Electrokit, SE). **Don't use cells under about 80–100 mAh**: the XIAO charges at 50 mA, keep it at ≤ 0.5–1 C. | | https://www.electrokit.com/ | not checked today |

---

## Tools you need

| Tool | Probably already have? | If not |
|---|---|---|
| Soldering iron (fine tip) + thin solder with flux | maybe | ≈ €15–25 locally (est.), or a makerspace (soldering stations at the Espoo makerspaces **unverified**) |
| Thin wire (silicone 28–30 AWG, or wire cut from an old cable) | likely | ≈ €2–5 locally (est.) |
| **USB-C data cable** (not charge-only) | yes | — |
| Laptop with Bluetooth (for the test receiver) | yes | — |
| Side cutters / tweezers / Kapton or electrical tape | likely | — |
| Multimeter | maybe | useful for polarity and battery checks, not required |

---

## If you already have an ESP32-S3 (free hold-to-talk test)

The repo's ESP32 sketch [`firmware/firmware/agent_pendant_v0_nohaptic`](firmware/firmware/agent_pendant_v0_nohaptic/agent_pendant_v0_nohaptic.ino) runs on an **ESP32-S3**. First check what you have:

- **It must say ESP32-S3.** A plain ESP32, ESP32-C3 or ESP32-C6 won't run this sketch as is. (Check the chip marking, or run `esptool.py chip_id`, or the board name in the listing.)
- Flash/PSRAM size (16 MB / 8 MB like in your friend's video) **does not matter** here. The sketch uses about 583 kB of flash and no PSRAM. That only mattered for the offline "Whistle" speech model in the video, which we are not using (8–12 s per command, about 78 % right on simple colour commands; our pendant sends audio to the phone and the agent instead).
- **The sketch expects a PDM microphone** (CLK on GPIO7, DATA on GPIO8), not an I2S mic.

What to buy on top, depending on the mic:

| Item | Why | Approx. price | Status |
|---|---|---|---|
| **PDM mic module** (e.g. MP34DT05 breakout) | works with the sketch **as is** | ≈ €3.68 (https://www.aliexpress.com/item/1005012782353411.html) | **Oct 27** Ali card |
| *or* INMP441 I2S mic | the one in the video; **needs a small code change** (switch `ESP_I2S` from PDM RX to standard I2S mode, add the WS pin, ≈ 30–60 min) | ≈ €1.56 (https://www.aliexpress.com/item/1005012438620524.html) | **Oct 27** Ali card |
| 6 × 6 mm button | hold to talk (or a jumper wire to GND for a first test) | €0.13 (BerryBase) | live |
| CO2 module (same STCC4 module as above) | the ESP32 can't measure CO2 by itself | ≈ €12.79 + duty | see Order 2 |
| Battery + charger | **only if** you want to wear it. A bare ESP32-S3 DevKit has no LiPo charger: run it from a USB power bank for the test. Avoid cheap TP4056 boards with small cells (they charge at 1 A). A Seeed XIAO ESP32S3 has a charger built in. | — | — |

Each extra AliExpress item type adds €3 duty. A free-ish ESP32-S3 test is therefore roughly: mic + CO2 module on one AliExpress order ≈ €20–25 (est.), or **€0** if you test hold-to-talk only with a mic you already have.

ESP32-S3 is a test bench, not the MVP: it draws much more idle power than the nRF52840, so it's not what we'd wear for a week.

## CSV
Machine-readable version: [`ORDER_LIST.csv`](ORDER_LIST.csv).
