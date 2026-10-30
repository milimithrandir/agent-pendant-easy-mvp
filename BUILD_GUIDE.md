# Easy MVP build guide

Board: **Seeed XIAO nRF52840 Sense**. Firmware: [`firmware/firmware/agent_pendant_v0_slim_nrf52/agent_pendant_v0_slim_nrf52.ino`](firmware/firmware/agent_pendant_v0_slim_nrf52/agent_pendant_v0_slim_nrf52.ino) (compiles, verified 2026-10-26 with Seeeduino:nrf52 1.1.13 + Sensirion I2C STCC4 1.1.0; **never run on real hardware yet**).

Everything below follows the pin definitions in that sketch.

## 1. Wiring

| XIAO nRF52840 Sense | nRF pin | Connects to | Notes |
|---|---|---|---|
| **3V3** | — | CO2 module **VCC / VDD** | 3.3 V only |
| **GND** | — | CO2 module **GND**, button leg B | common ground |
| **D4 (SDA)** | P0.04 | CO2 module **SDA** | default `Wire` pins |
| **D5 (SCL)** | P0.05 | CO2 module **SCL** | |
| **D1** | P0.03 | button leg A | `PIN_TALK`, internal pull-up; pressed = LOW = talk |
| **D2** (optional) | P0.28 | privacy slide switch → GND | `PIN_PRIVACY`; switch closed = mic off. **Leave unconnected** if you have no switch (reads as privacy OFF) |
| **BAT+ / BAT− pads** (underside) | — | LiPo red (+) / black (−) | **check polarity with a multimeter first** |
| (optional) | — | small slide switch in the BAT+ wire | the XIAO has no power switch; without one, unplug the cell to turn it off |
| onboard PDM mic | P1.10 power | — | nothing to wire; it's on the Sense board. Keep its opening uncovered |
| onboard RGB LED | — | — | status (see §4) |

```
            XIAO nRF52840 Sense (top view, USB-C up)
   CO2 SDA ── D4                 5V
   CO2 SCL ── D5                 GND ── CO2 GND, button B
   button A ─ D1                 3V3 ── CO2 VCC
  (privacy) ─ D2   ...
                 underside: BAT+ ── LiPo red   BAT− ── LiPo black
```

**Button legs:** a 4-leg 6 × 6 mm button connects its legs in pairs. Use two **diagonally opposite** legs and it will always work.

**CO2 module expectation:** the sketch talks to the STCC4 at I2C address **0x64** and expects the humidity sensor to sit on the STCC4's own secondary bus (as on the Adafruit 6478). The AliExpress module may be wired differently: check it with the scanner in §6 before you solder everything up.

## 2. Assembly, step by step

Do it in this order so each step is tested before the next one makes things harder.

1. **Flash the bare XIAO on USB** (§3). No soldering yet. You should see the blue LED for about 20 s (CO2 conditioning; it will print `STCC4 conditioning error` without a sensor, that's fine), then a green 30 ms blip every 4 s (advertising).
2. **Pair and test audio with a temporary button**: touch a wire from D1 to GND to "press". Run the laptop test (§5). This proves BLE, pairing and the mic before anything is permanent.
3. **Solder the button**: D1 to one leg, GND to the diagonal leg. Short wires.
4. **Solder the CO2 module**: 4 wires (3V3, GND, SDA→D4, SCL→D5). Run the I2C scan (§6), then the CO2 test.
5. **Battery last** (read §7 first): check polarity, cut and solder **one wire at a time**, red to BAT+, black to BAT−, insulate each joint with tape or heat-shrink. Optional: a slide switch in the red wire.
6. **Stack it**: cell at the back, a layer of Kapton/electrical tape, XIAO in the middle, CO2 module facing outward with its sensor opening uncovered and as far from the XIAO's chip and the battery as you can (they warm the air and skew CO2/temp). Keep the mic and the USB-C port reachable.
7. **Wear test**: a pouch / heat-shrink with a window over the CO2 sensor and the mic, on a cord. Later: the free library-printed case.

## 3. Flash the firmware

### Before you compile: set your own pairing PIN
Open the sketch and change line 31:
```c
#define PAIR_PIN "472913"     // change this
```
to your own 6-digit number, e.g. `"583016"`. Keep the quotes. **Don't keep 472913**: it's public in this repo, and anyone who knows it can pair with your pendant and listen.

**Never add code that sets P0.13 LOW.** It switches the XIAO charger from 50 mA to 100 mA, which is too much for a 100 mAh cell. The sketch deliberately leaves it alone.

### Option A: Arduino IDE (easiest)
1. File → Preferences → Additional boards manager URLs: `https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json`
2. Boards Manager → install **"Seeed nRF52 Boards"** (the sketch uses Bluefruit; **not** the "Seeed nRF52 mbed-enabled Boards" package).
3. Board: **Seeed XIAO nRF52840 Sense**.
4. Library Manager → install **"Sensirion I2C STCC4"** (it pulls in "Sensirion Core"). Bluefruit and PDM come with the board package. No SHT4x library is needed for the Adafruit-style wiring.
5. Open the sketch, set `PAIR_PIN`, click Upload. Serial Monitor at **115200** baud.

### Option B: arduino-cli
```bash
arduino-cli config add board_manager.additional_urls https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
arduino-cli core update-index
arduino-cli core install Seeeduino:nrf52
arduino-cli lib install "Sensirion I2C STCC4"
pip install adafruit-nrfutil            # Linux: the uploader needs it on PATH (and python)
cd firmware/firmware
arduino-cli compile -b Seeeduino:nrf52:xiaonRF52840Sense agent_pendant_v0_slim_nrf52
arduino-cli upload  -b Seeeduino:nrf52:xiaonRF52840Sense -p /dev/ttyACM0 agent_pendant_v0_slim_nrf52   # macOS: /dev/cu.usbmodem*
arduino-cli monitor -p /dev/ttyACM0 -c baudrate=115200
```
If the upload can't find the board: **double-tap the tiny reset button**. A USB drive called XIAO-SENSE appears and the port changes; upload again.

The sketch doesn't wait for the serial port, so you'll miss the first lines unless the monitor is already open. Press reset once with the monitor open.

## 4. What the LED means
| LED | Meaning |
|---|---|
| Blue, solid ~20 s after power-up | STCC4 conditioning (blocking) |
| Green 30 ms blip every 4 s | advertising, not connected |
| Off | connected and idle, or privacy ON |
| Red blinking (20 % duty) | recording |

If the colours look inverted (on when they should be off), change `LED_ON_LEVEL` at the top of the sketch.

## 5. Test hold-to-talk on a laptop
```bash
pip install bleak
python firmware/tools/pendant_rx.py
```
- It looks for a device named **"Agent Pendant"** for 20 s, then pairs.
- **macOS / Windows**: the OS asks for the passkey. Type your `PAIR_PIN`.
- **Linux**: if no prompt appears, pair once by hand: `bluetoothctl` → `agent KeyboardOnly` → `default-agent` → `scan on` → `pair <MAC>` → type the PIN → `quit`, then run the script again.
- **Hold** the button (at least 150 ms; shorter taps are ignored), talk, release. The red LED blinks while recording. The script prints `saved rec_0.wav (x.x s)` into the current folder. Play it.
- Dropped packets print `N frame(s) lost` and are filled with silence.
- The script also prints `state: recording=… batt=… mV` and CO2 lines when they arrive.

## 6. Test the CO2 sensor

### 6a. Is it wired, and how? (2 minutes)
Flash this tiny I2C scanner first (new sketch, same board):
```cpp
#include <Wire.h>
void setup(){ Serial.begin(115200); while(!Serial); Wire.begin();
  for(uint8_t a=1;a<127;a++){ Wire.beginTransmission(a); if(!Wire.endTransmission()) Serial.printf("found 0x%02X\n",a);} }
void loop(){}
```
| Result | Meaning | What to do |
|---|---|---|
| only **0x64** | STCC4 found, humidity sensor on its private bus (like Adafruit 6478) | firmware works as is |
| **0x64 and 0x44** | the SHT40 is on the main bus | CO2 still reads, but without humidity/temperature compensation (less accurate). About 1 h of firmware work: read the SHT40 (library "Sensirion I2C SHT4x") and pass its raw T/RH to `co2.setRhtCompensation(rawT, rawRH)` before each shot (ask Grok Bot) |
| **0x65** instead of 0x64 | the board uses a different address | in `setup()` change `co2.begin(Wire, STCC4_I2C_ADDR_64)` to `co2.begin(Wire, 0x65)` (the library only defines a constant for 0x64) |
| nothing | wiring, or no I2C pull-ups | check SDA/SCL aren't swapped, 3V3 and GND; if the module has no pull-up resistors, add 4.7 kΩ from SDA and SCL to 3V3 |

### 6b. Read CO2
Reflash the pendant firmware and keep the Serial Monitor open:
- After the ~20 s blue-LED conditioning, the first reading comes about 5 s later, then **every 5 minutes**: `CO2 812 ppm  T 23.4 C  RH 41.2 %  flags 00`.
- No reading is taken while you talk or for **120 s after** (your breath is about 40,000 ppm), so don't press the button while testing CO2.
- Sanity checks: outdoors or by an open window ≈ 420 ppm; a closed bedroom overnight often 1000+; breathing on it gives a big jump (then wait).
- Over 1000 / 1500 ppm the pendant sends an ALERT; `pendant_rx.py` prints it and pops a desktop notification.
- The STCC4 is specified for 10–40 °C, and its datasheet accuracy is ±(100 ppm + 10 %).

## 7. Safety (LiPo)
- **Check polarity** with a multimeter before soldering. Cheap cells don't always follow red = +. Reversed polarity can kill the XIAO.
- **Never short the cell.** Cut and strip one wire at a time, never both together. Insulate every joint. Keep metal tweezers off the pads.
- Don't solder onto the cell's metal tabs or the small protection board; solder to the leads, quickly.
- The XIAO charges at **50 mA** by default (fine for 100–150 mAh). **Never set P0.13 LOW** (100 mA).
- Charge from a normal USB port. Don't charge it unattended or under a pillow at first; stop if the cell swells, gets hot or smells.
- Don't bend, puncture or crush the cell in the case; leave a little room.
- Wearing it: it's a bare prototype. Keep it in a pouch or case so nothing shorts against skin, keys or a zip.

## 8. Troubleshooting
| Symptom | Likely cause | Fix |
|---|---|---|
| Upload fails / no port | charge-only cable, or not in bootloader | use a data cable; double-tap reset |
| Compile error about `bluefruit.h` | wrong board package (mbed) | install "Seeed nRF52 Boards" (Seeeduino:nrf52), pick XIAO nRF52840 Sense |
| `pendant not found` | not advertising, or bonded to another device | look for the green blip; remove "Agent Pendant" from the phone's/laptop's Bluetooth list; press reset |
| Pairing fails or no data after connecting | wrong passkey, old bond | use your `PAIR_PIN`; forget the device on both sides and pair again. Unpaired devices can connect but get nothing (by design) |
| No audio, red LED doesn't blink | button wiring | D1 must go LOW when pressed: check diagonal legs and GND |
| `PDM begin failed` | mic power / board variant | make sure the board is the **Sense** version and the board option is "XIAO nRF52840 Sense" |
| Choppy audio, many lost frames | weak link or MTU not raised | move closer; on Android apps call `requestMtu(247)`; the laptop script relies on the OS doing the MTU exchange |
| `STCC4 read error` / `wake error` | wiring, address, pull-ups | run the I2C scanner (§6a) |
| CO2 never prints | you pressed the button recently | wait 120 s without talking; readings are every 5 min |
| CO2 high and slow to fall | breath, or sensor in a closed pocket | give the sensor a window to open air, away from your mouth |
| Battery not charging | polarity or cell | check the XIAO's charge LED and the polarity |
| Battery mV looks odd | approximate divider maths | treat `batt` as a rough number; measure with a multimeter |
