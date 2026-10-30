# Agent Pendant: CHEAPEST V1 (research, 2026-10-27)

> **Copied into `agent-pendant-easy-mvp` on 2026-10-30 as background research.** The live order list is [`../../ORDER_LIST.md`](../../ORDER_LIST.md), re-checked 2026-10-30. Changes since this file was written: the BerryBase LP-402025 150 mAh cell is **sold out** (ORDER_LIST uses the LP-401230 100 mAh cell instead); the 6×6 H 5.0 mm button is down to 1 in stock (ORDER_LIST uses the H 4.3 mm one). Correction: the STCC4 is a **thermal-conductivity** CO2 sensor, not NDIR. It still measures CO2 directly (unlike eCO2/VOC sensors), but it is less accurate than NDIR/photoacoustic sensors (±(100 ppm + 10 %)).

> **Why this file exists.** Max (2026-10-27): "make mvp as cheap as possible ... version 1 maybe even cheaper". This compares the cheapest realistic ways to build a **v1 that proves hold-to-talk + CO2 in real life**, against the current custom-board minimum path ([`MINIMUM_PATH.md`](../v2-custom-board/MINIMUM_PATH.md), ≈ €170–330).
> Size still matters for the final product, but **for v1 cost wins**: v1 only has to be wearable enough to test, not jewelry.
> Nothing has been ordered or tested. Max orders everything himself.

Checked 2026-10-27, about 15:00–15:45 Europe/Helsinki.

**Labels**
- **(live)**: read today from the shop page / its product data.
- **(Ali card)**: AliExpress search-result card read today with ship-to Finland / EUR. It is the listing's *cheapest variant*; check that the variant you pick is the one listed. AliExpress shipping is not shown on the card (**unverified**, often €0–3).
- **(snippet)**: from a search summary or third-party page, not the vendor page.
- **(est.)**: my estimate.
- Customs (Finland, since 1 Jul 2026): parcels from outside the EU ≤ €150 pay **€3 duty per different item type** (not per unit, not per parcel) plus 25.5% VAT. AliExpress collects it at checkout (snippet, tulli.fi / Yle summary). Our earlier research also noted a further EU handling fee from 1 Nov 2026 (not re-verified today). **EU shops (BerryBase DE, Kiwi NL) have no duty**, so ordering *few item types* from AliExpress and the rest from the EU is cheapest.

---

## 0. The answer in one table

| Rank | Option | What you buy | Total delivered (est.) | Proves true CO2? | Firmware work | Bare size (est.) |
|---|---|---|---|---|---|---|
| **0** | **Step 0, €0: phone-only voice loop** | nothing | **€0** | no | small web page (PWA) | — |
| **1 (TOP PICK)** | **XIAO nRF52840 Sense + AliExpress STCC4/SHT40 module** | BerryBase (board, cell, buttons) + 1 AliExpress item | **≈ €49–56** | **yes (real CO2, thermal-conductivity sensor)** | **≈ none** (existing sketch), maybe 1 h if the module wires its SHT40 differently | ≈ 25 × 21 × 12–14 mm |
| 2 | "v1 good": same board + **Adafruit 6478** STCC4+SHT41 (EU, known-good) | BerryBase + Kiwi | ≈ €72–80 | yes | none (sketch was written for this board) | ≈ 27 × 23 × 13 mm |
| 3 | No-solder: **M5StickS3** + STCC4 (or SCD41) module | BerryBase + 1 AliExpress item | ≈ €52–60 | yes | ≈ ½ day port (ES8311 codec, power-hold pin) | 48 × 24 × 15 mm + sensor |
| 4 | Ultra-cheap: ESP32-C3 SuperMini + INMP441 + ENS160 eCO2 | all AliExpress | ≈ €35–42 | **no** (eCO2 is a VOC guess) | 2–4 h port + charger mod | ≈ 30 × 25 × 15 mm, messy |
| 5 | Cheaper custom board (1 JLC board, or bare PCB + hand solder) | JLC/LCSC + probe | ≈ €100–150 | yes | full port to bare module | Ø28 × 9–11 mm |
| 6 | Current minimum path (2 JLC boards) | see MINIMUM_PATH.md | ≈ €170–330 | yes | full port | Ø28.5 × 8.5–10.5 mm |

**Recommendation: do Step 0 today (free), order Option 1 (≈ €50), and park the custom board until v1 has proven the voice loop, the CO2 placement and the battery numbers.** Option 1 is ~€120–280 cheaper than the minimum path and runs the firmware that is already in this repo.

---

## A. Off-the-shelf stacks: boards and CO2 sensors

### A1. Boards with BLE + mic + LiPo charging

| Board | Mic | LiPo charging | Price, where | Notes |
|---|---|---|---|---|
| **Seeed XIAO nRF52840 Sense** | PDM onboard | yes (50 mA default) | **€19.90, BerryBase, 85 in stock (live)** https://www.berrybase.de/en/seeed-studio-xiao-nrf52840-sense-bluetooth-5.4-nfc-6-achsen-imu-mikrofon-imu-20-gpios-3-3v/auswahl-ohne-header · €20.16 Seeed store on AliExpress (Ali card) https://www.aliexpress.com/item/1005006988944671.html · Kiwi €14.99 ex VAT **out of stock** (live) | **Existing sketch `firmware/firmware/agent_pendant_v0_slim_nrf52` targets exactly this board.** Best BLE power of the lot. 21 × 17.8 mm |
| Seeed XIAO ESP32S3 Sense | PDM on the camera add-on board | yes | €17.90 BerryBase **sold out** (live) · €16.35 AliExpress (Ali card) https://www.aliexpress.com/item/1005007598922725.html | The ESP32-S3 sketch exists (`agent_pendant_v0_nohaptic`), only the mic pins change. Higher idle power than nRF52, and the camera board adds height |
| **M5StickS3** (ESP32-S3) | MEMS mic via ES8311 codec | yes, 250 mAh cell **built in** | **€24.90 BerryBase, 33 in stock (live)** https://www.berrybase.de/m5stack-m5sticks3-esp32s3-mini-iot-dev-kit-8mb-flash-8mb-psram-1-14-zoll-lcd-wifi-2-4-ghz-5v · $21.50 M5Stack store (live; import VAT/duty extra) · €24.62 OpenELAB (snippet) | Finished plastic case, 3 buttons, screen, speaker, Grove port. **No soldering.** 48 × 24 × 15 mm, 20 g: a "gadget", not a pendant |
| M5Stack Atom Echo | PDM mic | **no battery** | $13.50 M5Stack store (live) | Needs USB power or an extra battery base. Not worth it |
| nRF52840 SuperMini (nice!nano clone) | **none** | yes (charge current unverified) | €6.32 (Ali card) https://www.aliexpress.com/item/1005012371049017.html | Same chip as the XIAO; add an MP34DT05 PDM module €3.68 (Ali card) https://www.aliexpress.com/item/1005012782353411.html. Saves ≈ €10 vs the XIAO but adds 5 mic wires, a board-definition change and clone risk |
| ESP32-C3 SuperMini | **none** | **no** | €4.13 (Ali card) https://www.aliexpress.com/item/1005008234672660.html | Needs INMP441 I2S mic €1.56 (Ali card) https://www.aliexpress.com/item/1005012438620524.html and a TP4056 charger €0.69–1.21 (Ali card). **TP4056 boards charge at 1 A by default: dangerous for a 100–200 mAh cell unless you swap the PROG resistor.** |

### A2. CO2 sensors: honest comparison

| Sensor | What it measures | Price, where | Verdict for v1 |
|---|---|---|---|
| **STCC4 + SHT40 module** (Sensirion STCC4, true CO2, ~500 ms single shot, 1 µA sleep) | **real CO2** | **€12.79 (Ali card)** https://www.aliexpress.com/item/1005012973112114.html (new listing, **no sales yet**) · €13.76 alt. listing https://www.aliexpress.com/item/1005012807714972.html · ZY-STCC4 €14.56 (SHT4x not stated) https://www.aliexpress.com/item/1005012837347014.html | **Best fit**: smallest, lowest power, same chip as the custom board and **same library as the existing sketch**. Risk: unknown seller board layout (see §E3) |
| Adafruit 6478 STCC4 + SHT41 | real CO2 | **€24.49 ex VAT ≈ €30.74 incl. 25.5%, Kiwi (NL), 7 in stock (live)** https://www.kiwi-electronics.com/en/adafruit-stcc4-and-sht41-co2-temperature-amp-humidity-sensor-stemma-qt-qwiic-20617 | Known-good, the sketch was written for it. Bigger board. The "good" option |
| SCD40 / SCD41 modules (Sensirion photoacoustic) | real CO2 | AliExpress €11.69–24 (Ali cards, e.g. €11.69 https://www.aliexpress.com/item/1005010612621865.html, €15.40 https://www.aliexpress.com/item/1005007925654431.html, TENSTAR €22.72) · EU: Pimoroni SCD41 €49.90, Adafruit SCD-41 €52.90 at BerryBase (live via search) · M5 CO2 Unit (SCD40) $33.90, CO2L (SCD41) $45.90 (live) | Works, but **only the SCD41 has single-shot mode** (5 s per shot, ~0.5 mA average at 5-min intervals, unverified; SCD40 has to run periodic mode at mA level). 10.1 × 10.1 × 6.5 mm sensor, thicker than STCC4. Needs a library swap (≈ 1–2 h). Fallback if the STCC4 module is a dud. Reported AliExpress fakes: SCD41s without the laser data-matrix on the cap (HN, 2024) |
| SGP30 / ENS160(+AHT21) "eCO2" | **VOCs**, converted to an *estimated* CO2 | ENS160+AHT21 €5.49 (Ali card) https://www.aliexpress.com/item/1005009142706535.html · SGP30 €4.59–8.81 (Ali cards) | **Not true CO2.** The number follows VOCs (perfume, cooking, alcohol, cleaning sprays) and drifts with its own baseline; it can say 400 ppm in a stuffy room or 2000 ppm next to hand sanitiser. Fine for testing the data pipeline, **useless for "is this room stuffy"**. Don't build v1 on it |
| MH-Z19C (NDIR) | real CO2 | €8.44–23 (Ali cards) https://www.aliexpress.com/item/1005001883572051.html | Real CO2, but 33 × 20 × 9 mm, 5 V, ~40 mA peaks. Too big and hungry for a wearable. Desk-only |

### A3. Cheapest *viable* vs cheapest *good*
- **Cheapest viable (true CO2): Option 1, ≈ €49–56** (§E).
- **Cheapest good (known-good sensor, all EU, no duty): Option 2, ≈ €72–80**: BerryBase order of §E + Adafruit 6478 from Kiwi (€30.74 incl. VAT + PostNL ≈ €6–10, snippet) instead of the AliExpress module.
- Cheapest at all (eCO2): Option 4, ≈ €35–42, but it **can't validate CO2**, needs a charger hack, and the €3-per-item-type duty on 5–6 AliExpress item types (€15–18) eats most of the saving.

---

## B. Validate first with zero or near-zero hardware

Things Max very likely already has (not asked, just listed): a smartphone, a laptop, USB-C cables and charger, Bluetooth earbuds or headphones with a mic, maybe a power bank and a lanyard/necklace cord. Possibly a soldering iron and multimeter.

**Step 0a, €0, today: the voice/agent loop on the phone alone.**
- A one-page web app (PWA) with one big **hold-to-talk** button: `pointerdown` starts `MediaRecorder`, `pointerup` stops and uploads the clip to the agent; the reply comes back as a phone notification / text.
- Use the **earbuds' mic** (the browser uses the active Bluetooth headset mic) to feel how it is to talk to the agent without holding the phone to your mouth.
- This proves the part that matters most (is hold-to-talk to an agent useful day to day? latency? what do you say to it?) **before buying anything**. The pendant later just replaces the phone mic with a BLE mic.
- The same page is the seed of the **phone receiver** v1 needs anyway: on Android, Chrome supports Web Bluetooth, so the PWA can receive the pendant's BLE audio and CO2 directly. On iPhone, Safari has no Web Bluetooth; use the free Bluefy browser for testing or a small native app later (unverified that Bluefy handles the encrypted/passkey characteristics; nRF Connect works for raw checks).
- Today only a laptop receiver exists (`firmware/tools/pendant_rx.py`, Python + bleak).

**Step 0b, CO2 on its own.** When the v1 parts arrive, run the board + STCC4 on USB first, read CO2 in Serial Monitor, and compare: outdoors (~420 ppm), closed bedroom overnight (typically 1000+), car with recirculation. That validates the sensor and the case vent before anything is worn. No extra purchase.

**What Step 0 can't tell you:** BLE audio quality and range, battery life, how a pendant feels to wear and press.

---

## C. Cheaper custom-board routes (and whether they're worth it)

Numbers reuse the live JLC figures in `MINIMUM_PATH.md` (2026-10-26) unless marked.

| Route | What | Total (est.) | Problems |
|---|---|---|---|
| C1. **1 assembled JLC board instead of 2**, 2-layer | parts ≈ $28.80, setup + stencil ≈ $10.2, 4 extended-part fees ≈ $12.3, X-ray 4–5 leadless parts ≈ $7, PCB $2–7, shipping ≈ $7–20 (unverified), + 25.5% VAT + €3 duty | **≈ €75–100 for the board** + Pi Debug Probe €17–22 + cell €3–10 + pogo cable €3–6 → **≈ €100–140** | Fixed fees don't shrink with quantity, so 1 board saves only ≈ €35–60 vs 2. **No spare**: one bad joint or a dead module and you reorder (+2–3 weeks). 2-layer weakens the antenna ground (see MINIMUM_PATH §3) |
| C2. **JLC "basic parts only"** | — | not possible | The module, STCC4, mic and SHT40 are all **extended** parts at JLC. You can cut to ~3–4 extended parts, not 0 |
| C3. **Bare PCBs ($2 for 5, 2-layer ≤ 100 × 100 mm) + hand-solder parts from LCSC** | PCB $2 + shipping (unverified, $5–10); LCSC: BMD-340 $9.74 or E73 module (€6.84 Ali card), STCC4 $12.56, SHT40 $1.92, mic $3.86, charger/LDO/passives ≈ $2, LCSC shipping (unverified) + VAT + €3 × several item types; + hot plate / hot-air tool €20–40 (est.), paste, stencil; + Pi Debug Probe | **≈ €90–140** | **STCC4, SHT40, the mic and the module are all leadless LGA/DFN.** They can't be soldered with an iron; you need paste + stencil + hot plate, and a first-timer's yield is low. Likely to cost more than C1 after one failed attempt |
| C4. PCBWay / JLC new-user promos | coupons exist | **unverified** (not checked today) | Don't plan around them |
| Ref. Current minimum path | 2 boards, probe, 1 print | ≈ €170–330 | — |

**Honest verdict: a custom board is not worth it for v1.** It mostly buys *small size*, which v1 explicitly doesn't need. Every number the board design depends on (does push-to-talk feel right, mic quality through a case hole, how the CO2 reads on your chest, real battery drain) is cheaper to learn on a €50 XIAO stack. Do the custom board as v2, with the v1 data, and the €170–330 minimum path stays valid for that.

---

## D. Cheapest enclosure / way to wear it

| Option | Cost | Notes |
|---|---|---|
| **Espoo City Library makerspace, Sello or Iso Omena 3D printers** | **€0** | "Using the Makerspace and its appliances is free" (live, espoo.fi); reserve via Varaamo; PLA (search summary says material is free at Sello). Close to Kauniainen. **Recommended**: I can make the STL for the Option 1 stack |
| Clear heat-shrink + cord | €0–2 | Clear 2:1 heat-shrink 25–30 mm, €1.37/m (Ali card) https://www.aliexpress.com/item/32800208314.html, or a hardware store. **Cut a window over the CO2 sensor** and the mic, or it reads stale air. Ugly but works for a first week |
| Badge-holder / lanyard pouch (mesh or clear) | €1–3 (est.) | Air reaches the sensor through mesh. Most "invisible" cheap option. Press the button through the pouch |
| Small pill box / jewelry box, drilled | €1–5 (est., local shops) | Drill a vent and a button hole |
| Pendant bezel blanks | €2–3 for 5–10 pcs (Ali cards) | **Poor fit**: trays are 2–3 mm deep, the stack is 12–14 mm thick. Skip |
| JLC3DP resin | ≈ €3–10 + €3 duty (snippet: a France order of 2 tiny resin parts was €2.66 incl. shipping, before the July 2026 duty) | Nicer finish, 1–2 weeks. Only worth it if added to another JLC order |
| Finnish print service (tulostan.fi "from €8.99", snippet) | €9–25 | Fast, local, not needed if the library works |

---

## E. Top pick: exact buy list (Option 1)

### E1. Order 1: BerryBase (Germany, EU, no duty). DHL to Finland **€9.90** (orders €0.01–199.99, from BerryBase's shipping terms via search)

| Qty | Item | Price | Link | Status |
|---|---|---|---|---|
| 1 | Seeed XIAO nRF52840 Sense, without headers | €19.90 | https://www.berrybase.de/en/seeed-studio-xiao-nrf52840-sense-bluetooth-5.4-nfc-6-achsen-imu-mikrofon-imu-20-gpios-3-3v/auswahl-ohne-header | live, 85 in stock |
| 1 | LiPo LP-402025, 3.7 V **150 mAh**, JST (4 × 20 × 25 mm) | €3.30 | https://www.berrybase.de/en/lp-402025-lithium-polymer-lipo-battery-3.7v-150mah-with-2-pin-jst-connector | live price; stock not read |
| 2 | Tact switch 6 × 6 mm, H 5.0 mm (1 spare) | 2 × €0.09 | https://www.berrybase.de/en/short-stroke-pushbutton-vertical-pcb-mounting-6x6mm-h-5.0mm | live price |
| | Shipping | €9.90 | | snippet |
| | **Subtotal** | **≈ €33.28** | | |

Prices shown include German VAT; a Finnish delivery may be re-priced to 25.5% at checkout (≈ +€1.50, unverified). **Unverified: whether BerryBase ships LiPo cells to Finland** (lithium cells are often restricted in cross-border parcels). Fallbacks: Electrokit 40 mAh 55 SEK (in MINIMUM_PATH) or a 402030 200 mAh cell on AliExpress, €5.33 (Ali card) https://www.aliexpress.com/item/1005010776246310.html (+€3 duty as a 2nd item type).

### E2. Order 2: AliExpress (one item type = one €3 duty)

| Qty | Item | Price | Link | Status |
|---|---|---|---|---|
| 1 | STCC4 CO2 module with onboard SHT40 | €12.79 | https://www.aliexpress.com/item/1005012973112114.html | Ali card; new listing, 0 sold |
| | Duty (collected at checkout) | €3.00 | | rule (snippet) |
| | Shipping | €0–3 | | **unverified** |
| | **Subtotal** | **≈ €15.79–18.79** | | |

Buy 2 if you want a spare sensor: still one item type, so still one €3 duty (≈ +€12.79).

### E3. Already owned / free
Soldering iron + solder, a USB-C cable, thin wire, a cord. Case: free library print (§D). If you lack a soldering iron, a basic one is ≈ €15–25 locally (est.), or use the makerspace (the Espoo page lists crafts/tools; soldering stations not verified).

**Top pick total: ≈ €49–52 + any VAT re-pricing ≈ €49–56.**

### E4. What to check when it arrives
1. **STCC4 module address and SHT40 wiring.** The sketch expects the STCC4 at I2C **0x64** with the SHT4x on the STCC4's own secondary bus (as on the Adafruit 6478). If this module puts the SHT40 on the main bus instead, the sketch needs ≈ 1 h of changes (read the SHT40 and pass RH/T to the STCC4). Not knowable from the listing.
2. Wire: STCC4 module VDD → XIAO 3V3, GND, SDA → D4, SCL → D5; button D1 → GND; LiPo to the XIAO BAT+/BAT− pads (red to +). Same as the sketch header.
3. **Never set P0.13 LOW** (that switches the XIAO to 100 mA charging; 50 mA default is right for 150 mAh).
4. Flash over USB-C with Arduino (Seeed nRF52 core), no debug probe needed. Test with `tools/pendant_rx.py` on the laptop first.

### E5. Size and battery (est.)
- Stack: XIAO 21 × 17.8 mm + 150 mAh cell 20 × 25 × 4 mm + STCC4 module (size not listed; ZY-STCC4 is 15 × 11.5 mm) → **≈ 25 × 21 × 12–14 mm bare, ≈ 30 × 26 × 16 mm in a printed case.** About a 2 € coin footprint, chunky like a big locket.
- Battery: 150 mAh → **≈ 3–7 days** at 10 min talk/day (est.; the XIAO's LEDs, IMU and regulator idle higher than the custom board; measure with a multimeter in series).

### E6. What Option 1 can't validate
- The **final size/look** (that is the custom board's job).
- Antenna performance in a small Ø28 case on the body, the magnetic pogo charging, the top-port mic acoustics of the custom design.
- **Final battery life** (custom board idles lower).
- The CO2 vent geometry of the final case (it does validate CO2 *on the body*, roughly).
- iPhone support until a phone receiver exists (Android Web Bluetooth PWA is the cheapest route).

---

## F. Firmware effort per option

| Option | Starting point | Effort (est.) |
|---|---|---|
| 1 / 2: XIAO nRF52840 Sense + STCC4 | `firmware/firmware/agent_pendant_v0_slim_nrf52` (hold-to-talk, PDM, passkey BLE, non-blocking CO2) | **0** for Adafruit 6478; **0–1 h** for the AliExpress module (SHT40 wiring, §E4) |
| SCD41 instead of STCC4 | same sketch | 1–2 h (Sensirion SCD4x library, 5 s single shot, keep it non-blocking) |
| XIAO ESP32S3 Sense | `agent_pendant_v0_nohaptic` (ESP32-S3, PDM via ESP_I2S, NimBLE) | ≈ 30 min (mic pins GPIO42/41 on the Sense board; verify) |
| M5StickS3 | ESP32-S3 sketch | ≈ ½ day: ES8311 codec init (M5Unified), button mapping, power-hold pin, screen optional |
| ESP32-C3 + INMP441 | ESP32-S3 sketch | 2–4 h: I2S standard mode instead of PDM, pins, single-core timing; plus TP4056 resistor swap |
| nRF52840 SuperMini + MP34DT05 | nRF52 sketch | 1–2 h: board definition (nice!nano / Adafruit core), `PDM.setPins()`, LED/charge pins |
| Custom board (C1–C3) | nRF52 sketch | 1–3 days: board definition, bare-module bring-up via SWD, power gating |
| **Phone receiver (all options)** | none yet (laptop `pendant_rx.py` only) | ≈ 1 day for an Android PWA (Web Bluetooth + upload to the agent + notifications) |

---

## G. Suggested sequence
1. **Today, €0:** phone PWA hold-to-talk test with earbuds (Step 0a).
2. **Today:** order E1 (BerryBase) and E2 (AliExpress). ≈ €50. BerryBase ≈ 3–5 business days (est.), AliExpress ≈ 1–3 weeks (est.).
3. While waiting: I write the Android PWA receiver and the case STL for the library printer.
4. On arrival: CO2 on USB (Step 0b) → wire → wear for a week → log what breaks.
5. Then decide v2: the custom board in `MINIMUM_PATH.md`, sized with real data.

**Unverified summary:** AliExpress shipping on each card; whether the €12.79 STCC4 module is genuine and how its SHT40 is wired (new listing, no sales); BerryBase LiPo shipping to Finland and VAT re-pricing; Bluefy/iOS behaviour with the passkey-protected characteristics; the 1-board JLC total (est. from yesterday's live line items, not a quote); JLC3DP and Finnish print prices (snippets); battery-life numbers.
