# LATEST and probably best specs so far (2026-10-26)

Status: most recent, probably best specs so far. Supersedes the pebble (58x46x18) and round (Ø52x18) studies for planning purposes. The slim Ø35x16 CAD in `cad/round-slim-v1` is still the Path 1 enclosure, but it models a 402020 120 mAh cell; the recommended Electrokit 55 mAh cell is 15x10x5 mm, so rework the battery tier before printing. All sizes marked "est." are estimates. Nothing has been ordered or tested on hardware yet.

## Product definition (MVP)
- Push-to-talk voice: hold button, record only while held, compressed audio over BLE to the phone, then to the AI agent.
- CO2 (+ temperature/humidity) read only every 5-10 min (duty-cycled, STCC4 single-shot), not continuous.
- No vibration. All notifications and CO2 alerts go through the phone.
- Privacy: software mic disable (onboard mic cannot be power-cut on the XIAO Sense); custom board can add a hardware switch later.

## Path 1: DIY bench MVP (no custom PCB), order now, est. 1-2 weeks to first prototype
- Seeed XIAO nRF52840 Sense (recommended over nRF54L15: same size when hand-built, nRF54L15 charges at ~200 mA = unsafe for tiny cells, community-only Arduino core, no proven BLE mic streaming). EU: BerryBase 19.90 EUR incl. DE VAT; Seeed direct $15.90.
- Adafruit 6478 STCC4 + SHT41 breakout (Kiwi, 24.49 EUR excl. VAT, ships same day).
- 55 mAh LiPo 15x10x5 mm with protection (Electrokit, 55 SEK). Do not use a 40 mAh cell limited to 40 mA charge. Never drive XIAO pin P0.13 LOW (switches charge to 100 mA).
- Slide switch (Kiwi 0.99 EUR), 6x6 tact switch, thin silicone wire, 3D-printed case.
- Est. size: ~Ø32-34 x 15 mm (Adafruit board) or ~25x21x12 mm with the smaller ZY-STCC4 + SHT40 (humidity sensor not confirmed on ZY board).
- Cart: ~EUR 75-95 fast (EU) or ~EUR 45-60 cheap (China, 1-3 weeks; each non-EU parcel adds 3 EUR duty per category + VAT).

## Path 2: Custom PCB (JLCPCB assembly), est. Ø26-28 x 9-10 mm, est. 3-5 weeks
Verified LCSC/JLC parts (Oct 26 2026, qty-5 price):
| Role | Part | LCSC | $ | Note |
|---|---|---|---|---|
| BLE MCU | Ebyte E73-2G4M08S1C (nRF52840, 18x13) | C356849 | 7.69 | only nRF52840 module in stock at JLC; Ø24 needs 8x8 ISP1807-LR-RS (C5264268, 0 stock, global sourcing) |
| CO2 | Sensirion STCC4-D-R3 | C49191471 | 12.56 | thermal-isolation slot; case vent grille above it |
| T/RH | Sensirion SHT40-AD1B-R2 | C2909890 | 1.92 | wired to STCC4 SDA_C/SCL_C |
| PDM mic | Knowles SPH0641LU4H-1 | C2879853 | 2.09 | bottom port, 1 mm hole in PCB; cheaper alt MSM261DDB020 C27636198 $0.65 (type unconfirmed) |
| Charger | MCP73831T-2ACI/OT | C424093 | 0.70 | R_PROG 33-47 kOhm (21-30 mA) |
| LDO 3.3 V | XC6206P332MR-G (or MCP1700T-3302E/TT C39051) | C5446 | 0.15 | basic part |
| PTT button | ALPS SKRPACE010 | C139797 | 0.07 | |
| Battery (separate) | 40 mAh LiPo with PCM, ~4x10x17 mm | - | ~2-4 | no EU shop found with exact cell |
- Load-sharing MOSFET+diode dropped (device charges off the neck).
- Parts per board ~USD 25-26 (est.). 5 assembled boards ~USD 170-250 (est.); 2 assembled boards ~USD 105-125 reported by another AI, unverified.
- Corrections to an earlier AI-generated BOM: C2688382, C2909967, C1927027 were wrong parts (see option2_bom_factcheck.md).

## Corrections to Path 2 (review 2026-10-26)
Checked against datasheets and the JLC parts API; the table above is kept as researched.
- E73-2G4M08S1C is 13x18x**3.0 mm** with a **ceramic chip** antenna (not a PCB trace) and an inner grid of pads under the module (no hand rework). Stack becomes ~10.5 mm with a 4 mm cell.
- The E73 needs an **external 32.768 kHz crystal** on XL1/XL2 (datasheet v2.4), e.g. C32346.
- Smaller, certified alternative in stock: u-blox **BMD-340-A-R** (C5456944, 15.0x10.2x1.9 mm, FCC 2AA9B10, CE RED), 4.4 mm RF keep-out at the board edge.
- Realistic board with the full BOM (crystal, TVS, STAT divider, privacy switch): BMD-340 Ø28 single-sided / Ø26-27 double-sided; E73 Ø30 single-sided / Ø28-29 double-sided. Case ~Ø30-32 x 9-10.5 mm.
- A bottom-port mic on a top-mounted board ports into the battery. Use a top-port PDM mic on top (e.g. MSM261DHT006, C51928215) or mount the SPH0641 on the bottom side.
- MCP73831-2AC terminates at 7.5 % of I_REG (1.6 mA at 21 mA); any docked load near that can stop termination. Use the -2AT (20 %, C14879) with R_PROG 47 kOhm, or BQ25180 for v2. Don't wire STAT (up to ~5 V) straight to a GPIO.
- Missing from the table: 32 kHz crystal + caps, SWD pads, ESD on the charge pads, STAT divider or VBUS sense, test points.
- No board wash / no ultrasonic cleaning (STCC4 and MEMS mic datasheets): put it in the JLC order remark.
- A hardware mic cut is possible on the custom board: DPDT slide switch (e.g. C221665) cutting mic VDD and PDM DATA.
- 5-10 min single shots are within the STCC4's recommended 5-600 s interval for ASC; only the accuracy figure is specified at 10 s.

## Known risks
- STCC4 accuracy is only specified at 1 reading per 10 s; 5-10 min intervals may be noisier. Works 10-40 C; keep away from body heat; needs vent.
- Tiny-cell charge current; antenna keep-out at board edge; battery sets final thickness (~9-10 mm).
- ZY-STCC4 board humidity sensor unconfirmed.

## Files in this folder
research_smallest.md, mvp_feasibility.md, option2_bom_factcheck.md, concept images in images/ (AI concept art, approximate sizes), plus the slim CAD (cad/round-slim-v1) and no-haptics firmware (firmware/).
