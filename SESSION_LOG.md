# Session log: decisions with Max

Dates are approximate (from file times of the work), Europe/Helsinki time. Everything is design and research: **nothing has been ordered or tested on hardware.** Max orders all parts himself.

| Date (approx.) | Decision / event |
|---|---|
| 2026-10-22 | Project start from `specs.md` / `ideas.md` ([`original/`](original/)): CO2 sensor + microphone, wearable, practical, "somewhat aesthetic". Max asks to source from China where cheaper. First parts list and size estimate. |
| 2026-10-23–24 | First CAD and concept renders, then a **round** case like the Grok Bot logo (≈ Ø52 mm). Delivery-time estimates. |
| 2026-10-24 | Max: the Ø52 version is **too big for an amulet**. **No vibration** (no LRA / haptic driver). **Hold-to-talk**: records only while the button is held. **All notifications go through the phone.** New slim round case **Ø35 × 16 mm** and no-haptics firmware (ESP32-S3 and XIAO nRF52840 Sense). |
| 2026-10-25 | Worn-on-a-person concept images for scale. Front holes reworked into a cleaner **dot pattern**, speak button kept. Max: still too big, "it's quite a statement to wear something big around your neck". **CO2 only every 5–10 min**, no continuous monitoring. Research into the smallest possible hardware (bare STCC4, nRF52840 modules). |
| 2026-10-26 | Parts checked for EU/China availability and self-build feasibility. Reviewed another assistant's Option 1 (hand-built stack ≈ €35) and Option 2 (custom board) proposals and corrected the BOM. Specs pushed to the original repo, then a review PR (non-blocking CO2, passkey pairing, power-saving connection parameters) merged. Option 1 shown at real size (≈ 25 × 21 × 12 mm): **Max rejected Option 1 for size and chose Option 2** (custom board). Plan written: BMD-340 module, Ø26 board, ≈ Ø28.5 × 8.5 mm, in the new Grok Bot version repo. Max asked why the budget (≈ €400–550) was so high and chose the **minimum path** (2 boards, Pi Debug Probe only, ≈ €170–330). |
| 2026-10-27 | Max: make the MVP **as cheap as possible**, v1 maybe even cheaper. Research ranks a **≈ €49–56** XIAO nRF52840 Sense + STCC4 module build (existing firmware) as the top pick, plus a **free phone-only Step 0**. Custom board becomes v2. |
| 2026-10-30 | A friend's video: offline "Whistle" speech-to-text on an ESP32-S3 with an INMP441 mic. Max has an ESP32 at home. Verdict: an ESP32-S3 can run the repo's ESP32-S3 sketch for a hold-to-talk bench test, but the offline model isn't useful here (8–12 s per command, ~78 % on simple commands); the CO2 module is still needed. |
| 2026-10-30 | **Max: "let's do the easy MVP."** This repo created with the order list (re-checked live today), build guide, Step 0 and the v2 path. Live re-check found the 150 mAh cell sold out (switched to 100 mAh) and the H 5.0 mm button nearly sold out (switched to H 4.3 mm). |
| 2026-10-30 | Max wants the parts **as soon as possible** while still keeping costs down. CO2 board switched from the AliExpress STCC4 module (1–3 weeks, new seller) to the **Adafruit 6478 from Kiwi Electronics (NL)**: same-day shipping, zero firmware changes. Total ≈ €70–75 instead of ≈ €49–56. Board stays at BerryBase: Kiwi's XIAO Sense is out of stock until 2026-11-06, Robomaa (FI) only has the non-Sense XIAO. |
| 2026-10-30 | Goal clarified: **voice input should go to Max's Grok Bot automatically.** Max uses the Grok Bot app on Mac and phone today. Research found Grok Bot **webhook routines** (official): the bridge POSTs the transcript to the routine's URL and the Bot replies in its normal chat. Max doesn't want to pay for a speech API, so speech-to-text runs **free on device** (Whisper on the Mac first; Apple/Android speech on the phone later). Plan in [`GROK_BOT_BRIDGE.md`](GROK_BOT_BRIDGE.md). |

## Standing requirements
- Hold-to-talk only; mic records only while held. No vibration. No screen, no speaker.
- Notifications and CO2 alerts go to the phone.
- CO2 every 5–10 min (firmware: every 5 min, burst of 3, never within 120 s after talking).
- Final product small and discreet (v2 target ≈ Ø28.5 × 8.5–10.5 mm). v1 may be bigger.
- Cheapest path first; custom board later.
