# Agent Pendant, Option 2: MINIMUM PATH (chosen)

> **Decision 2026-10-26: Max chose the minimum path.**
> This file is the plan to follow. The full plan in [`option2_endtoend_plan.md`](option2_endtoend_plan.md) is kept as the **recommended / extended alternative**: 5 boards, the nRF52840 DK, PPK2 and 2–3 case prints. Its design work (architecture, schematic, layout rules, bring-up checklist) still applies unless this file says otherwise.

Checked 2026-10-26, about 18:40 Europe/Helsinki. Nothing has been ordered or tested on hardware. Max orders and pays for everything himself.

**How to read the labels**
- **(live)**: read today from the vendor page or the JLCPCB parts API.
- **(est.)**: my estimate, not a vendor figure.
- **(unverified)**: I couldn't confirm it from a primary source today.
- **(snippet)**: the number comes from a search summary or a third-party page, not the vendor's own page.
- Currency: €1 = $1.1188 (ECB, 2026-10-26), so $1 ≈ €0.894.

Companion BOM: [`option2_bom_minimum.csv`](option2_bom_minimum.csv).

---

## 1. Headline

| | Minimum path (chosen) | Extended plan (alternative) |
|---|---|---|
| Assembled boards | **2** | 5 |
| JLC order, delivered to Finland | **≈ €131–182** (est.). Goods value ≈ €90–110, so it is **IOSS-eligible (≤ €150)** either way | ≈ €255–315 |
| Programmer | **Raspberry Pi Debug Probe, €11.90** | nRF52840 DK + Pi probe, ≈ €77 |
| Case prints | **1 round** | 2–3 rounds |
| **Total** | **≈ €170–330.** About €200–250 is likely if you already own a soldering iron and a multimeter | ≈ €400–550 |
| Time to a wearable prototype | ≈ 5–6 weeks (est.) | ≈ 5–6 weeks |

**Does the JLC order stay under €150?**
- **Customs threshold: yes.** The goods value is about €90–110 before VAT and shipping, so it stays inside the EU's ≤ €150 B2C/IOSS rules:
  - JLC collects the 25.5% VAT at checkout;
  - you pay €3 temporary duty per HS code plus the €2 Union Handling Fee per line on import (UHF starts 2026-11-01).
- **Delivered total: only with the cheapest options.** It lands at about €131–150 with Global Standard Direct Line shipping and a cheap PCB finish. With DHL and a pricier finish it can reach about €182. That is still IOSS, but over €150 out of pocket. **Pick Global Standard Direct Line and the cheapest flat finish (see §3.3).**
- Source for the customs rules: https://jlcpcb.com/help/article/eu-customs-reform-faq (updated 2026-10-26, live).

---

## 2. What to order and buy, in order

| # | When | What | Where | Cost (low–high) | Status |
|---|---|---|---|---|---|
| 1 | Week 1 | **Raspberry Pi Debug Probe** (SC0889). Includes a micro-USB cable and 3 debug cables | welectron (DE) https://www.welectron.com/Raspberry-Pi-Debug-Probe_1 | €11.90 + shipping ≈ €5–10 → **€17–22** | price and stock (100+) **live**; shipping to FI **unverified** |
| 2 | Week 1 | **2 × LiPo 40 mAh 20.5 × 10 × 3 mm with PCM** (Akyga LP301020), 1 per board | Electrokit (SE, ships to FI and charges Finnish VAT) https://www.electrokit.com/en/battery-lipo-3.7v-40mah-bec | 2 × 55 SEK ≈ €10 + shipping ≈ €6–12 → **€16–22** | price 55 SEK and stock 18 **live**; shipping **unverified** (shown at checkout) |
| 3 | Week 1 | **2-pin magnetic pogo charging cable, male + female set** (USB-A to 2 pins, 2.54 mm pitch) | AliExpress | **€3–6** | **snippet** (≈ $1.90–3.04 per set); arrives in 1–3 weeks |
| 4 | Week 1, only if missing | Fine-tip soldering iron, flux, tweezers, Kapton tape, 30 AWG wire, **multimeter with a µA range** | local / EU | **€0–70** | est. Skip anything you already own |
| 5 | End of week 2 | **JLC order: 2 assembled boards** (fabricate 5, assemble 2), Economic PCBA, plus **one case print round** in the same cart (JLC3DP resin) | jlcpcb.com + jlc3dp.com | **€131–182** boards + **€5–25** case | see §3 |
| — | Optional | XIAO nRF52840 Sense, to test BLE audio before the boards arrive (§5.3) | TME FI | €27.26 gross | **snippet**; not in the total |

**Total (est.)**

| | Low | High |
|---|---|---|
| JLC 2-board PCBA, delivered | 131 | 182 |
| Pi Debug Probe + shipping | 17 | 22 |
| 2 × 40 mAh cells + shipping | 16 | 22 |
| Magnetic 2-pin pogo cable set | 3 | 6 |
| Case print, 1 round | 5 | 25 |
| Basic hand tools (only if you lack them) | 0 | 70 |
| **Total** | **≈ €172** | **≈ €327** |

---

## 3. The JLC order in detail

### 3.1 Cuts vs the extended plan (BOM changes)

**Extended parts drop from 7 to 4, saving 3 × $3.07 = $9.21 per order.**
- **Charger:** MCP73831 (C424093, extended) → **TP4054-42-SOT25R (C32574)**, which is a **"preferred extended" part: no loading fee in Economic PCBA** (live; JLC FAQ: "Preferred Extended Parts … are exempt from the Feeders Loading fee").
  - Same SOT-23-5 pinout:
    - pin 1 CHRG / STAT;
    - pin 2 GND;
    - pin 3 BAT;
    - pin 4 VCC;
    - pin 5 PROG.
  - The footprint does not change (TOPPOWER datasheet, live).
  - **I = 1000 V / R_PROG** for currents up to 150 mA, so **47 kΩ gives about 21 mA.**
  - Datasheet changes:
    - VCC wants **≥ 10 µF**, so the charger input cap becomes 10 µF (C15525, already a BOM line).
    - **CHRG is open-drain only.** That's fine for the LED, and the GPIO needs a pull-up.
    - In sleep the BAT pin draws **< 2 µA**.
  - The charge-current accuracy at 21 mA is **unverified**: the datasheet only specifies 10 kΩ and 1.66 kΩ.
- **LED:** NCD0402R1 (0402, extended) → **KT-0603R (C2286, basic)**. It's 0603, so about 0.8 mm² more area.
- **ESD diode D1:** **not fitted** (the footprint stays, marked DNP). It saves $3.07.
  - Risk: a static zap on the exposed charge pad goes into the charger.
  - The VBUS_DET line is already behind a 100k resistor.
  - Hand-solder D1 later if you see failures.
- **Unchanged (no basic alternative exists):**
  - BMD-340 (C5456944, **416 in stock, $9.74**, live);
  - STCC4 (C49191471, 1,473, $12.56);
  - SHT40 (C2909890, 18,521, $1.92);
  - mic SPK0838HT4H-1 (C5447637, **37 in stock**, $3.86). The fallback is IM69D130V01, C536262, 246 in stock.

**Parts per board ≈ $28.80** (was $29.35). All prices are live, from the JLCPCB parts API, 2026-10-26.

### 3.2 2 layers vs 4 layers: stay with 4

- **On cost:** 2 layers would save only about $5. A third-party live quote on 23 Aug 2026 for a 100 × 100 mm board, 5 pcs, OSP finish, was **$2 for 2 layers vs $7 for 4 layers** (snippet: https://comparepcb.com/articles/surface-finish-costs-more-than-you-think).
- **On risk:** 2 layers would put the plan at risk.
  - The BMD-340 can be fanned out on 2 layers (datasheet Fig. 6).
  - On a Ø26 disc, though, about 25 parts plus the STCC4 thermal slot and the antenna keep-out would chop up the only ground plane. The module's antenna and the 2.4 GHz return paths depend on that plane.
- **Verdict: 4 layers, 0.8 mm.** The finish is what moves the price (§3.3), not the layer count.

### 3.3 Surface finish: the biggest hidden cost

The same third-party live quote (23 Aug 2026, 4 layers, 100 × 100 mm, 5 pcs, at JLC) gave:

| Finish | Price |
|---|---|
| OSP | **$7.00** |
| HASL | $72.70 |
| Lead-free HASL | $77.90 |
| ENIG | $90.30 |

A Ø26 board will be cheaper, but the ratio probably holds. **This is unverified for our size; the quote page shows it once the Gerbers are uploaded.**

| Finish | Fits LGA/DFN (BMD-340, STCC4, SHT40, mic) | Cost on 4 layers at JLC | Verdict |
|---|---|---|---|
| **OSP** | **Yes, flat pads.** Fine for a single reflow straight into JLC assembly | cheapest | **Default for the minimum path, if the quote page offers it at 0.8 mm (unverified)** |
| ENIG | yes, flat; best shelf life; gold contact pads | about 10× OSP (snippet) | Fallback if OSP isn't offered at 0.8 mm. It can still stay under €150 goods |
| HASL / lead-free HASL | **No.** Uneven pads on 0.4–0.5 mm-pitch LGA/DFN invite opens and tilt | expensive on 4 layers anyway | Don't use |

- **The only OSP downside here is the back-side charge and SWD pads.** They are bare copper under a thin organic film and will tarnish.
- **The fix costs nothing:**
  - tin the two charge pads with the iron after delivery;
  - better still, glue the **female half of the magnetic pogo set into the back cap** and wire it to the pads with 2 short wires. Its gold contacts and magnet then give the alignment as well.
- Sensirion doesn't mandate ENIG for STCC4/SHT40 as far as I found (unverified). A flat finish and no washing are what matter (§3.5).

### 3.4 X-ray: can't be skipped

JLC's price page (live, updated 2026-10-22) says X-ray is **required** for "BGA, QFN, and other leadless packages".
- We have 4 leadless parts per board: BMD-340 LGA, STCC4, SHT40 DFN and the mic.
- With 2 boards that's **8 × $1.64 = $13.12**. If they also count the FC-135 crystal it's 10 × $1.64 = $16.40.
- Assembling 2 boards instead of 5 is the only real saving here.

### 3.5 Cost of the 2-board order (est.; fee lines live)

| Line | USD |
|---|---|
| Parts, 2 × $28.80 | 57.6 |
| Setup $8.18 + stencil $1.53 + confirm placement $0.45 | 10.2 |
| 4 extended parts × $3.07 | 12.3 |
| SMT joints, ≈ 2 × 160 × $0.0016 | 0.5 |
| X-ray, 8–10 leadless × $1.64 | 13.1–16.4 |
| Attrition / minimum-buy extras (est.) | 0–6 |
| PCB 4L 0.8 mm OSP, 5 pcs (ENIG pushes the top of this range; est.) | 7–20 |
| Shipping: Global Standard Direct Line (low) to DHL (high), est. | 12–35 |
| **Subtotal before VAT** | **≈ $113–158 (≈ €101–141)** |
| Goods value without shipping, used for the €150 test | ≈ €90–110: **under €150** |
| Finnish VAT 25.5% (IOSS, at checkout) | ≈ €26–36 |
| €3 temporary duty per HS code + €2 UHF per line (paid on import, not through IOSS) | ≈ €5 |
| **Delivered** | **≈ €131–182** |

Adding the case print to the same cart likely adds a second HS code, so another €3 duty plus €2 UHF (est.).

### 3.6 Order settings (Max)

1. jlcpcb.com → upload `gerbers.zip`. Choose:
   - 4 layers, 0.8 mm;
   - **OSP** (ENIG if OSP isn't offered);
   - qty 5 (the minimum fab quantity);
   - "Remove Mark", or set a mark location.
2. Tick **PCB Assembly** → **Economic** → top side → **assemble 2**.
3. Upload the BOM and CPL. Check:
   - that every line matches its C-number in `option2_bom_minimum.csv`;
   - **D1 is "Do not place".**
4. Tick **Confirm Parts Placement ($0.45)**. Check pin 1 of the BMD-340, STCC4, SHT40, mic and charger.
5. Add the remark: **"STCC4/SHT40/MEMS mic: do not wash, no ultrasonic cleaning, no conformal coating; no-clean flux."**
6. Optional: from the JLC3DP page, add the case print to the **same cart** (JLC3DP describes a combined cart and payment).
   - That article is from Aug 2025, and a search summary claims newer JLC3DP guidance disallows combining (**unverified**).
   - If the cart refuses, print in the EU instead (§6).
7. Shipping: **Global Standard Direct Line**, which is always handled as B2C/IOSS per JLC's FAQ. Check that the goods value is ≤ €150.
8. Pay. Answer the DFM/engineering email quickly.

---

## 4. Charging: 2 pads + a cheap magnetic pogo cable

- **The cheapest working setup is about €3–6:** a **2-pin magnetic pogo set** (male cable + female socket), 2.54 mm pitch, from AliExpress (snippet).
- On the board: **2 large back pads** (VBUS, GND), each 1.5–2 mm.
- In the case: the female socket is glued into the back cap and wired to the pads. Alternatively, the cable's spring pins land directly on tinned pads.
- **Buy the cable first. I copy its pitch and outline into the PCB and case before the JLC order.**
- Charge rate: 21 mA on a 40 mAh cell (0.5C). Electrokit's sheet allows up to 1C (40 mA) and calls 0.2C (8 mA) standard.
- **The charger has no thermistor input, so never charge while wearing it** (unchanged).

---

## 5. Programming and debugging with only the Raspberry Pi Debug Probe

### 5.1 Hardware hookup

- **Board side:** 1.0 mm SWD test pads on the back: **3V3, GND, SWDIO, SWCLK**, plus RESET (P0.18) as an optional pad.
  - Add a **UART TX test pad** (any free GPIO) for serial logs. It's free on the PCB.
- **Probe side:** the probe's **"D" (debug) JST-SH port.** Use the included JST-to-0.1" cable:

| Pi probe wire | Board pad |
|---|---|
| SC (orange) | SWCLK |
| SD (yellow) | SWDIO |
| GND (black) | GND |

- Easiest physical connection: solder 3 short 30 AWG wires to the pads and end them in a 3-pin 0.1" header. Remove them before closing the case.
- Serial logs: the probe's **"U" port** (UART, 3.3 V). Wire probe RX to the board's TX pad and GND to GND. It shows up as a second USB serial port at 115200 baud.
- **The probe does not power the target.** Power the board from its battery, or from 5 V on the charge pads. The probe's I/O is for 1.8–3.3 V targets only; our 3V3 rail is fine. **Never connect the probe to the 5 V pad.**

### 5.2 Software (OpenOCD ≥ 0.12)

**Install OpenOCD:**
- macOS: `brew install open-ocd`
- Linux: `sudo apt install openocd`. Check `openocd --version` is ≥ 0.12.
- Windows: the xPack OpenOCD build.

The probe is a standard CMSIS-DAP v2 adapter. How-to sources:
- https://jsan.ch/posts/2023/09/programming-an-nrf52-with-the-raspberry-pi-debug-probe/
- https://marceloborges.dev/posts/1/

```bash
# helper: Pi Debug Probe (CMSIS-DAP) + nRF52 target
ocd() { openocd -f interface/cmsis-dap.cfg -c "transport select swd" -c "adapter speed 1000" -f target/nrf52.cfg "$@"; }

# 1. Does the probe see the chip?
ocd -c "init; exit"

# 2. (optional, only if the chip is not locked) save u-blox's public BLE address (UICR CUSTOMER[0..1]);
#    it is also printed on the module label
ocd -c "init; mdw 0x10001080 2; exit"

# 3. Unlock / erase (needed if APPROTECT is active on newer nRF52840 silicon; erases everything)
ocd -c "init; nrf52_recover; exit"

# 4. Flash SoftDevice S140 6.1.1 + the Arduino app (no bootloader on the minimum path)
ocd -c "init; reset halt; nrf5 mass_erase; program s140_nrf52_6.1.1_softdevice.hex verify; program agent_pendant.ino.hex verify; reset; exit"
```

- **The app hex:**
  - Arduino IDE with the Adafruit nRF52 core.
  - Use board "Adafruit Feather nRF52840 Express" until our `agent_pendant_bmd340` variant exists. The pin map differs, so only use it for an LED/BLE smoke test.
  - Then **Sketch → Export Compiled Binary** gives the `.hex`.
- **The SoftDevice:** the S140 v6.1.1 hex from Nordic's S140 download page. It's the version the Adafruit core links against.
- **No bootloader at first:**
  - All updates go over SWD. That keeps the minimum path simple.
  - The Adafruit bootloader (for BLE OTA DFU, or USB DFU if D+/D− pads exist) can be added later.
  - **Unverified:** whether the SoftDevice's MBR boots the app at 0x26000 cleanly with no bootloader present. I expect it to; it gets checked at the first LED blink.
- **Debug:** run `ocd` with no extra command and leave it running, then `arm-none-eabi-gdb agent_pendant.ino.elf` → `target extended-remote :3333` → `monitor reset halt`, `break setup`, `continue`.
- **Gotchas:**
  - If the probe connects once but fails after a reset, APPROTECT is re-locking. Re-run step 3 before every flash. Some projects, e.g. RIOT, always recover before flashing.
  - `nrf52_recover` erases the UICR, including u-blox's BLE address. A random static address is fine for us.

### 5.3 Firmware work without the nRF52840 DK

| Before the boards arrive, with **no extra hardware** | With the **optional XIAO nRF52840 Sense** (≈ €27) | **Only possible on the real boards** |
|---|---|---|
| The firmware compiles for nRF52840 (Arduino CLI / CI) | The existing sketch runs unchanged: passkey pairing, hold-to-talk PDM → BLE audio to the phone / `tools/pendant_rx.py`, connection parameters, phone-side app | Mic **power gating** by the button (hardware privacy check) |
| The custom variant compiles (pin map, `USE_LFXO`, VBAT ×2) | Practising the Pi probe + OpenOCD flow on the XIAO's SWD pads | 32 kHz crystal start-up (20 pF caps) |
| Phone/receiver code tested against recorded audio files | Rough idle and talking current with a multimeter | TP4054 charging and termination, CHRG pin |
| STCC4 driver logic against a mock (no sensor: the Adafruit 6478 breakout is dropped) | — (still no CO₂ without a sensor) | **Real CO₂ readings**, thermal-island behaviour, vent design |
| | | Antenna range on the Ø26 ground plane, worn on the body; real battery life |

**My suggestion:** skip the XIAO if money is the priority. Buy it if you want BLE audio de-risked before the €150 board order; it's the single biggest firmware risk.

---

## 6. Case: one print round

- Design the case around the KiCad STEP export (week 2), so the first print matches the real board.
- Cheapest decent option: **JLC3DP SLA resin (8001/9600)**, added to the JLC cart.
  - Resin starts **"from $0.30 per part"**, with no minimum quantity and a minimum part size of 5 × 5 × 5 mm (JLC3DP help page; snippet).
  - Front + back shell for 2 devices: about $3–10 (est.).
- **EU alternative** if the carts can't combine: Craftcloud (quote on upload) or Tulostan.fi ("from €8.99", snippet). Either is about €10–25 delivered (est.), with no customs.
- Dropped: rounds 2–3. Any fit issue gets fixed with a file and some foam tape, or a second print later.

---

## 7. What is deliberately dropped, and the risk each cut adds

| Dropped | Saves (≈) | Risk it adds | Cheap mitigation |
|---|---|---|---|
| **3 spare assembled boards** (5 → 2) | €100+ | One board damaged during bring-up or rework leaves you with one. A design bug costs a full re-order (≈ €130+) | Bring up board #1 slowly with a current-limited supply. 3 bare PCBs come anyway (fab qty 5) |
| **nRF52840 DK** | €65 | No reference board for firmware before the PCBs; no J-Link | Pi probe + OpenOCD (§5). Optional XIAO Sense (€27) |
| **PPK2 power profiler** | €130 | **Battery life stays unverified.** No µA-accurate current profile | **Multimeter in series with BAT+** (below). Plus VBAT logging over days |
| **Adafruit 6478 STCC4 breakout** | €31 | CO₂ code untested until the boards arrive | The firmware CO₂ path is small and already reviewed. Test it first in bring-up |
| **Extra case print rounds** | €20–75 | The first print may not fit or look right | Design around the real STEP. Keep the print cheap |
| **55 mAh cell and the 3rd cell** | ≈ €5–10 | Less runtime; no spare cell | 40 mAh still gives ≈ 18–30 days at 10 min/day (est.) |
| **ESD diode D1** (not fitted) | $3 | A static discharge on the charge pad could kill the charger | The footprint stays. Hand-fit later if needed |
| **ENIG finish** (→ OSP) | $10–80 (est.) | Bare-copper back pads tarnish | Tin the pads, or use the gold contacts of the magnetic socket |
| **USB data pads** (default 2 pads, see §9) | €0 (small area) | No USB/UF2 updates; SWD only | SWD + later BLE OTA DFU |

**Measuring current without a PPK2** (Pi probe can't measure current):
1. Put the multimeter **in series with BAT+**. Start on the **mA range**, with a jumper wire across the meter during boot and BLE connect so radio bursts don't brown it out.
2. Once idle, remove the jumper and switch to the **µA range** to read sleep and idle current. Targets: ≤ 30 µA connected idle; ≈ 3–6 mA while talking (est.).
3. Readings jump around during BLE events. Take the average of several readings, or add a 100–470 µF cap across the board's battery input so the meter sees an averaged current.
4. Long-term check: the firmware reports VBAT over BLE. Log it for 3–5 days of normal use and extrapolate.

---

## 8. Timeline (est.)

| Week | Dates | Max | Grok Bot |
|---|---|---|---|
| 1 | Oct 26 – Nov 1 | Answer the 2 decisions (§9). Order the Pi probe, 2 cells and the magnetic pogo set (+ tools / XIAO only if needed). Send me the pogo set's listing | KiCad 10 schematic with the minimum BOM (TP4054, KT-0603R, D1 DNP, UART TX + SWD pads) |
| 2 | Nov 2 – 8 | Review the layout PDF and 3D. **Place the JLC order** (2 boards + case) by about Nov 8, while the BMD-340 (416) and mic (37) are in stock | Ø26 4-layer layout, DRC, BOM/CPL export; case CAD around the STEP; pogo pitch copied in |
| 3 | Nov 9 – 15 | Install OpenOCD, check the probe enumerates (and flash the XIAO if bought) | `agent_pendant_bmd340` variant + sketch port; flash script; bring-up checklist |
| 4 | Nov 16 – 22 | Boards in transit (Global Standard ≈ 7–12 days, est.) | Bring-up and current-measurement scripts |
| 5 | Nov 23 – 29 | **Bring-up board #1** (§5, plus extended plan §5.2), then board #2. Fit the case | Fixes |
| 6 | Nov 30 – Dec 6 | Wear test, VBAT log, CO₂ vs a reference meter | Rev-B notes if needed |

---

## 9. Open decisions (with my recommended default)

1. **Back pads: 2 or 4?** **Default: 2 pogo pads (VBUS, GND).** Put D+/D− on two small test pads next to the SWD pads (free) instead of on the pogo cable.
   - Programming is SWD on this path anyway.
   - 2-pin magnetic cables are the cheapest and most common.
   - The free D+/D− test pads keep USB possible later.
   - Pick 4 only if you want routine USB updates through the magnetic cable; that needs a 4-pin cable.
2. **Approve the BMD-340 at Ø26?** **Default: yes.** Order by about Nov 8.
   - 416 in stock at JLC and $9.74 (live today).
   - Fallback: E73 on a Ø28 board.
3. (New, minor) **Finish:** default **OSP**, falling back to ENIG if OSP isn't offered at 0.8 mm.
4. (New, minor) **D1 ESD diode not fitted:** default yes, to save $3.07.

---

## 10. What Max does first

1. Reply with the 2 decisions: **2 or 4 back pads**, and **BMD-340 at Ø26 approved**. If you're happy with my defaults, "defaults OK" is enough.
2. Order the **Raspberry Pi Debug Probe** from welectron (€11.90 + shipping).
3. Order **2 × Electrokit 40 mAh LiPo (LP301020)**.
4. Order a **2-pin magnetic pogo charging cable, male + female set**, from AliExpress, and send me the listing link so I can copy its pitch.
5. Check you have: a fine-tip soldering iron, flux, tweezers, Kapton tape, 30 AWG wire and a multimeter with a µA range. Buy only what's missing.
6. Optional: a XIAO nRF52840 Sense if you want BLE audio tested before the boards.
7. Install OpenOCD on your computer.
8. When I post the layout (week 2): review it, then **place the JLC order** with the settings in §3.6. Check that goods are ≤ €150 and the shipping is Global Standard Direct Line.

---

## 11. Verified vs unverified

**Live today (2026-10-26):**
- JLC parts stock and prices for all BOM lines, including TP4054 C32574 (preferred extended, 48,695 in stock) and KT-0603R C2286 (basic).
- The JLC PCBA fee table and the rule that X-ray is required for leadless parts.
- The JLC EU customs FAQ.
- welectron Pi Debug Probe: €11.90, 100+ in stock.
- Electrokit 40 mAh: 55 SEK, 18 in stock.
- The TP4054 datasheet: pinout, I = 1000/R, ≥ 10 µF on VCC, < 2 µA in sleep.

**Unverified or estimated:**
- The exact 4-layer 0.8 mm PCB price for a Ø26 board, and whether OSP is offered at 0.8 mm. The finish ratios come from a third-party Aug 2026 quote.
- Shipping costs from JLC, welectron and Electrokit to Finland.
- Whether JLC counts the crystal for X-ray, and the attrition extras.
- Whether JLC3DP still combines with a JLCPCB cart.
- Magnetic pogo cable prices (search snippets).
- XIAO Sense price (TME snippet).
- The HS code count on the JLC parcel.
- TP4054 accuracy at 21 mA.
- MBR → app boot with no bootloader.
- All battery-life and current figures.
