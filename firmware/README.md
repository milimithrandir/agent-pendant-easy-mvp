# Agent Pendant v0 prototype (XIAO ESP32S3 + STCC4 + PDM mic + LRA)

## Wiring (XIAO ESP32S3)
| XIAO pin | GPIO | Goes to |
|---|---|---|
| 3V3 | - | STCC4 VIN, haptic 3-5V, mic 3V (through privacy slide switch) |
| GND | - | all GND, mic SEL, button |
| D4 | 5 | SDA -> STCC4 SDA, haptic SDA |
| D5 | 6 | SCL -> STCC4 SCL, haptic SCL |
| D8 | 7 | mic CLK |
| D9 | 8 | mic DAT |
| D1 | 2 | tactile button (other leg to GND) |
| BAT+/BAT- pads (underside) | - | LiPo (red to +). Charge via USB-C. |
| (optional) A0 | 1 | BAT+ -> 220k -> A0 -> 220k -> GND for battery voltage |

I2C addresses: STCC4 0x64 (SHT41 sits on STCC4's own secondary bus), DRV2605L 0x5A.

## Build
arduino-cli core install esp32:esp32   (3.3.x)
arduino-cli lib install "Sensirion I2C STCC4" "Adafruit DRV2605 Library" "NimBLE-Arduino"
arduino-cli compile -b esp32:esp32:XIAO_ESP32S3 firmware/agent_pendant_v0
arduino-cli upload  -b esp32:esp32:XIAO_ESP32S3 -p /dev/cu.usbmodem* firmware/agent_pendant_v0

## Test from laptop
pip install bleak && python tools/pendant_rx.py   # saves rec_N.wav, prints CO2

## BLE protocol
Service 6e400001-...-0001
- AUDIO ...0002 notify: [seq u16][pred i16][index u8][160 B IMA-ADPCM, 320 samples, 16 kHz, low nibble first]
- ENV   ...0003 notify/read: [co2 i16 ppm][temp*100 i16][rh*100 u16][flags u8: 1=exhale-suspect 2=>=1000 4=>=1500]
- CTRL  ...0004 write: 0x01 start rec, 0x00 stop


## No-haptics MVP firmware (hold-to-talk, phone alerts)

Two sketches, same BLE protocol, no DRV2605L/motor:

| Sketch | Board | Build |
|---|---|---|
| `firmware/agent_pendant_v0_nohaptic/` | XIAO ESP32S3 (existing Ø52 / pebble hardware minus haptics) | `arduino-cli compile -b esp32:esp32:XIAO_ESP32S3 firmware/agent_pendant_v0_nohaptic` |
| `firmware/agent_pendant_v0_slim_nrf52/` | XIAO nRF52840 **Sense** (Ø35 x 16 slim round case, onboard PDM mic) | add board URL `https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json`, `arduino-cli core install Seeeduino:nrf52`, then `arduino-cli compile -b Seeeduino:nrf52:xiaonRF52840Sense firmware/agent_pendant_v0_slim_nrf52` (needs `python` and `adafruit-nrfutil` on PATH) |

Verified compile 2026-10-26 (arduino-cli, esp32 core 3.3.12 + NimBLE-Arduino 2.5.2 / Seeeduino:nrf52 1.1.13, Sensirion I2C STCC4 1.1.0): ESP32S3 583 kB flash (17 %), with `-DHTT_TOUCH=1` 599 kB; nRF52840 127 kB (15 %). Still untested on hardware.

Behaviour
- **Hold to talk**: mic streams only while the button is held (>=150 ms to start, ignores taps); release = stop. On nRF the mic is powered (P1.10) from the moment of the press, and the first 150 ms are sent as pre-roll once the press counts, so the first syllable is kept. A 320 ms ring absorbs BLE stalls; overruns drop whole frames and the `seq` gap tells the receiver.
- **Privacy switch** (D2 -> GND = privacy ON): talk ignored, mic off. ESP32 build: the switch's 1st pole also cuts the Adafruit 3492 VDD (hardware). nRF Sense build: onboard mic -> firmware-enforced (MIC_PWR low, PDM stopped).
- **LED**: solid (ESP32) / blinking red at 20 % duty (nRF) while recording; 30 ms blip every 4 s while advertising; off when connected and idle or in privacy.
- **CO2 every 5 min**: burst of 3 single shots, first discarded, mean of the other two. No shot is taken while talking or for 120 s after (exhaled breath; STCC4 tau63 is 20 s), and each shot blocks 500 ms, so measuring mid-take would stall audio. A deferred burst is retried every 10 s; a burst aborts if the talk button is pressed. At boot the STCC4 runs `performConditioning()` (~22 s, blue LED on nRF).
- **CO2 alerts go to the phone**: ALERT notify at >=1000 ppm (level 1) and >=1500 ppm (level 2), 10 min cooldown per level. ENV flags gain 0x08 = "this sample raised an alert".
- **Pairing required**: every characteristic needs an encrypted, passkey-authenticated (MITM) bond. The phone/laptop asks for the 6-digit passkey `PAIR_PIN` (default `472913`; override with `--build-property compiler.cpp.extra_flags=-DPAIR_PIN=...` and change it per device). Unpaired centrals can connect but get nothing and cannot write CTRL.
- **Connection parameters**: the pendant asks for 45-60 ms with peripheral latency 30 (6 s timeout) while idle and 15-30 ms with no latency while the mic is on. It also requests 2M PHY, data-length extension and MTU 247 (nRF). Without the MTU exchange (Android: call `requestMtu(247)` in the app) the 165 B audio packets do not fit.
- **Power (nRF)**: DC/DC regulator enabled; idle loop polls every 50 ms; the privacy switch's pull-up is enabled only for the instant it is read (twice a second), because a permanent 13 k pull-up into the closed switch draws about 250 uA. Not done yet: putting the onboard QSPI flash into deep power-down (about 15 uA).
- ESP32 only: `-DHTT_TOUCH=1` uses D1 (GPIO2 / TOUCH2) as capacitive touch electrode (baseline calibrated at boot, touched = >1.25x baseline).

Wiring delta vs v0: remove DRV2605L + LRA; privacy switch 2nd pole D2 -> GND; hold-to-talk button D1 -> GND (unchanged). Slim nRF: STCC4 SDA D4, SCL D5, 3V3, GND (STEMMA connectors removed, wires soldered).

New characteristics (service `6e400001-a9e4-4c1b-9d3e-ae9e0a9e0001`)
- `...0005` STATE notify/read: `[recording u8][privacy u8][batt_mV u16 LE]` (batt = 0 on ESP32S3, approximate on nRF)
- `...0006` ALERT notify/read: `[level u8][ppm int16 LE]`
- `...0004` CTRL write: 0x00 stops a recording. 0x01 (remote start) is compiled out unless `-DCTRL_REMOTE_START=1` (debug only): a remote start would break the "records only while the button is held" promise.

`tools/pendant_rx.py` subscribes to STATE/ALERT when present, saves a WAV per take (filling dropped frames with silence) and pops a desktop notification for alerts. It calls `pair()` where bleak supports it; on macOS the OS prompts for the passkey on first access.
