# From the easy MVP to v2 (custom board)

The easy MVP is **v1**: cheap, bigger, built from off-the-shelf parts, and running the firmware already in this repo. **v2** is the small custom pendant. The plan for it is already written and parked here:

- [`docs/v2-custom-board/MINIMUM_PATH.md`](docs/v2-custom-board/MINIMUM_PATH.md): the cheapest custom-board route (chosen on 2026-10-26).
- [`docs/v2-custom-board/option2_endtoend_plan.md`](docs/v2-custom-board/option2_endtoend_plan.md): the full end-to-end plan (architecture, PCB, JLC ordering, bring-up, enclosure).
- BOMs: [`option2_bom_minimum.csv`](docs/v2-custom-board/option2_bom_minimum.csv), [`option2_bom.csv`](docs/v2-custom-board/option2_bom.csv), corrections in [`option2_bom_factcheck.md`](docs/v2-custom-board/option2_bom_factcheck.md).

## v2 in one paragraph (estimates from 2026-10-26, not re-checked)
A Ø26 mm, 4-layer, 0.8 mm board built around the pre-certified **u-blox BMD-340** nRF52840 module (15 × 10.2 × 1.9 mm), with the bare **STCC4 + SHT40**, a top-port PDM mic powered through the hold-to-talk button, a small charger, and charging through 2 back pads with a magnetic pogo cable. Finished pendant about **Ø28.5 × 8.5 mm** (40 mAh cell) or × 10.5 mm (55 mAh). Minimum path: 2 assembled JLC boards (≈ €131–182 delivered), a Raspberry Pi Debug Probe as the only programmer, 2 cells, a pogo cable and one case print: **≈ €170–330 total**, about 5–6 weeks. **Not started yet:** the schematic, the board layout, the v2 case CAD and the firmware port to the bare module. BMD-340 stock at JLC was 416 on 2026-10-26 (re-check before designing around it).

## What v1 has to teach us before v2 is designed
| Question | How v1 answers it | Why v2 needs it |
|---|---|---|
| **Is hold-to-talk useful day to day?** | Step 0 + a week of wearing v1 | if not, v2 changes (e.g. tap-to-toggle, or no pendant at all) |
| **CO2 placement** on the chest: does a vent read room air, or your breath and body heat? Is the 120 s exhale hold-off enough? | log CO2 while wearing vs. on the desk; compare with an open window | sets the vent position, the thermal slot and the hold-off in v2 |
| **Battery numbers**: real idle current and mAh/day at your talk time | multimeter in series; `batt` mV over days | picks 40 vs 55 mAh and the v2 thickness |
| **Mic through a case hole**: is the speech clear, how much clothing rustle? | record takes through the printed case | sets the mic port size/position (v2 uses a top-port mic) |
| **Antenna on the body**: range and dropouts phone in pocket / other side of the room | `N frame(s) lost` counts while wearing | the v2 Ø28 case and board-edge antenna keep-out |
| **Button feel**: force, position, accidental presses | wearing it | v2 button and cap |
| **iPhone-to-Grok Bot path**: does one button hold/release deliver a complete recording automatically with the phone locked, and does the Bot act on it? | build and wear-test the native iPhone receiver and Bot delivery route against v1 | preserve a proven BLE protocol and receiver for v2; do not assume a routine equals a normal voice message |
| **Privacy switch**: needed, or is "mic only powered while held" enough? | your call after wearing | v2 already cuts mic power with the button |

## Carry-over
- The **BLE protocol and iPhone receiver** should carry over if the locked-phone and automatic-delivery tests pass; change them if those tests expose a problem.
- The **firmware** carries over with a new board definition and pin map (bare BMD-340, flashed over SWD with the Pi Debug Probe).
- The CO2 logic (burst of 3 shots every 5 min, first discarded; hold-off after talking; phone alerts at 1000/1500 ppm) carries over, tuned with v1 data.
