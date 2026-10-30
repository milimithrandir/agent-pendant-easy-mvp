// Agent Pendant v0 SLIM (no haptics) - for the Ø35 x 16 mm slim round case
// Board: Seeed XIAO nRF52840 Sense (Seeed nRF52 core "Seeeduino:nrf52" 1.1.x, FQBN Seeeduino:nrf52:xiaonRF52840Sense)
//        board index: https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
// Parts: onboard PDM mic (powered via P1.10 only while talking), Adafruit STCC4+SHT41 (I2C 0x64, STEMMA removed,
//        wired direct to D4/D5/3V3/GND), 6x6 tact button D1->GND (hold to talk), privacy slide switch D2->GND,
//        onboard RGB LED behind a light pipe. No motor / DRV2605L.
// Libraries: "Sensirion I2C STCC4" (+ Sensirion Core). Bluefruit + PDM are part of the core.
//
// Same GATT protocol as agent_pendant_v0_nohaptic (ESP32-S3) so tools/pendant_rx.py works with both.
//  - HOLD TO TALK: mic powered on press; audio is sent only once the button has been held >= 150 ms, starting
//    from the press itself (pre-roll), so the first syllable is not lost. Release = stop.
//  - Privacy ON: mic power pin forced off, PDM never started, talk ignored (firmware-enforced; see README).
//  - LED: red blinks while recording, green 30 ms blip / 4 s = advertising, blue during boot conditioning.
//  - CO2: burst of 3 single shots every 5 min (first discarded, mean of the other two) -> ENV notify.
//    Never measured while talking or within 120 s after (exhaled breath, and the 500 ms blocking shot would
//    stall audio). ALERT characteristic notifies the phone at >=1000 / >=1500 ppm.
//  - BLE: bonding with a static passkey (PAIR_PIN) required for every characteristic. Long connection interval
//    with peripheral latency while idle, short interval while streaming.

#include <Arduino.h>
#include <Wire.h>
#include <bluefruit.h>
#include <PDM.h>
#include <SensirionI2cStcc4.h>

#define PIN_TALK     D1
#define PIN_PRIVACY  D2
#define LED_ON_LEVEL LOW      // XIAO nRF52840 RGB LED is wired active-LOW (variant.h says 1; verify on your board)

#ifndef PAIR_PIN
#define PAIR_PIN "472913"     // 6-digit static passkey entered on the phone when pairing; change per device
#endif
#ifndef CTRL_REMOTE_START
#define CTRL_REMOTE_START 0   // 1 = allow CTRL 0x01 to start recording from the phone (debug only)
#endif

static const uint32_t SAMPLE_RATE       = 16000;
static const uint32_t FRAME_SAMPLES     = 320;     // 20 ms
static const uint32_t RING_FRAMES       = 16;      // 320 ms: covers the 150 ms pre-roll plus BLE jitter
static const uint32_t RING_SAMPLES      = FRAME_SAMPLES * RING_FRAMES;
static const uint32_t CO2_PERIOD_MS     = 5UL * 60UL * 1000UL;
static const uint8_t  CO2_SHOTS         = 3;       // first shot discarded, the rest averaged
static const uint32_t EXHALE_HOLDOFF_MS = 120000;  // STCC4 tau63 is 20 s; ~95 % recovery takes ~60 s plus case delay
static const uint32_t CO2_RETRY_MS      = 10000;   // re-check this often when a shot was deferred
static const int16_t  CO2_WARN_PPM      = 1000;
static const int16_t  CO2_ALERT_PPM     = 1500;
static const uint32_t ALERT_COOLDOWN_MS = 10UL * 60UL * 1000UL;
static const uint32_t TALK_MIN_MS       = 150;
static const uint32_t PRIVACY_POLL_MS   = 500;

// Connection parameters (1.25 ms units, timeout 10 ms units), within Apple's accessory design guidelines
static const uint16_t IDLE_MIN_INT = 36, IDLE_MAX_INT = 48, IDLE_LATENCY = 30, IDLE_TIMEOUT = 600;  // 45-60 ms, 6 s
static const uint16_t TALK_MIN_INT = 12, TALK_MAX_INT = 24, TALK_LATENCY = 0,  TALK_TIMEOUT = 400;  // 15-30 ms, 4 s

#define F_EXHALE 0x01
#define F_WARN   0x02
#define F_ALERT  0x04
#define F_NEWALERT 0x08

// 6e4000XX-a9e4-4c1b-9d3e-ae9e0a9e0001, little-endian byte order for Bluefruit
static uint8_t uuidBytes[7][16];
static const uint8_t* uuid(uint8_t id) {
  static const uint8_t base[16] = {0x01,0x00,0x9e,0x0a,0x9e,0xae,0x3e,0x9d,0x1b,0x4c,0xe4,0xa9,0x00,0x00,0x40,0x6e};
  memcpy(uuidBytes[id], base, 16); uuidBytes[id][12] = id; return uuidBytes[id];
}
BLEService        svc;
BLECharacteristic audioChr, envChr, ctrlChr, stateChr, alertChr;

SensirionI2cStcc4 co2;
bool micOn = false, recording = false, privacy = false;
uint32_t lastSpeechEndMs = 0, nextCo2Ms = 0, lastAlertMs[3] = {0, 0, 0};
uint16_t frameSeq = 0;
uint16_t connHandle = BLE_CONN_HANDLE_INVALID;

// PDM -> ring of samples (written from the PDM callback, read in loop)
static int16_t pcmBuf[RING_SAMPLES];
volatile uint32_t pcmWrite = 0;   // samples written (monotonic)
uint32_t pcmRead = 0;
void onPdm() {
  int n = PDM.available();
  while (n > 0) {
    int16_t tmp[256];
    int b = PDM.read(tmp, min(n, (int)sizeof(tmp)));
    if (b <= 0) break;
    for (int i = 0; i < b / 2; i++) pcmBuf[(pcmWrite + i) % RING_SAMPLES] = tmp[i];
    pcmWrite += b / 2; n -= b;
  }
}

// ---------- IMA ADPCM ----------
static const int8_t idxTable[16] = {-1,-1,-1,-1,2,4,6,8,-1,-1,-1,-1,2,4,6,8};
static const int16_t stepTable[89] = {
 7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,50,55,60,66,73,80,88,97,
 107,118,130,143,157,173,190,209,230,253,279,307,337,371,408,449,494,544,598,658,
 724,796,876,963,1060,1166,1282,1411,1552,1707,1878,2066,2272,2499,2749,3024,3327,
 3660,4026,4428,4871,5358,5894,6484,7132,7845,8630,9493,10442,11487,12635,13899,
 15289,16818,18500,20350,22385,24623,27086,29794,32767};
struct AdpcmState { int16_t pred = 0; int8_t index = 0; } adpcm;
uint8_t adpcmEncodeSample(int16_t s) {
  int step = stepTable[adpcm.index];
  int diff = s - adpcm.pred; uint8_t code = 0;
  if (diff < 0) { code = 8; diff = -diff; }
  int delta = step >> 3;
  if (diff >= step) { code |= 4; diff -= step; delta += step; }
  step >>= 1; if (diff >= step) { code |= 2; diff -= step; delta += step; }
  step >>= 1; if (diff >= step) { code |= 1; delta += step; }
  int p = adpcm.pred + ((code & 8) ? -delta : delta);
  adpcm.pred = constrain(p, -32768, 32767);
  adpcm.index = constrain(adpcm.index + idxTable[code], 0, 88);
  return code;
}

// ---------- helpers ----------
void led(uint8_t pin, bool on) { digitalWrite(pin, on ? LED_ON_LEVEL : !LED_ON_LEVEL); }
uint16_t batteryMv() {
  // XIAO nRF52840: P0.31 via 1M/510k divider, enabled by VBAT_ENABLE (P0.14) LOW. Approximate.
  analogReference(AR_INTERNAL_3_0); analogReadResolution(12);
  uint32_t raw = analogRead(PIN_VBAT);
  return (uint16_t)(raw * 3000UL / 4096UL * 1510UL / 510UL);
}
void publishState() {
  uint16_t mv = batteryMv();
  uint8_t s[4] = {(uint8_t)recording, (uint8_t)privacy, (uint8_t)mv, (uint8_t)(mv >> 8)};
  stateChr.write(s, sizeof(s));
  if (Bluefruit.connected()) stateChr.notify(s, sizeof(s));
}
void micPowerOff() { pinMode(PIN_PDM_PWR, OUTPUT); digitalWrite(PIN_PDM_PWR, LOW); }

void requestConnParams(bool talk) {
  if (connHandle == BLE_CONN_HANDLE_INVALID) return;
  ble_gap_conn_params_t p;
  p.min_conn_interval = talk ? TALK_MIN_INT : IDLE_MIN_INT;
  p.max_conn_interval = talk ? TALK_MAX_INT : IDLE_MAX_INT;
  p.slave_latency     = talk ? TALK_LATENCY : IDLE_LATENCY;
  p.conn_sup_timeout  = talk ? TALK_TIMEOUT : IDLE_TIMEOUT;
  sd_ble_gap_conn_param_update(connHandle, &p);
}

// Mic on at press (so the 150 ms debounce becomes pre-roll); frames are only sent once recording starts.
void micStart() {
  if (micOn || privacy) return;
  PDM.onReceive(onPdm);
  PDM.setBufferSize(FRAME_SAMPLES * 2);
  if (!PDM.begin(1, SAMPLE_RATE)) { Serial.println("PDM begin failed"); return; }  // also powers mic (P1.10)
  pcmRead = pcmWrite;
  micOn = true;
  requestConnParams(true);   // ask for the short interval early; the phone takes a few events to switch
}
void micStop() {
  if (!micOn) return;
  PDM.end(); micPowerOff();
  micOn = false;
}

void setRecording(bool on) {
  if (on && (privacy || !micOn)) on = false;
  if (on == recording) {
    if (!on && micOn) { micStop(); requestConnParams(false); }   // tap shorter than TALK_MIN_MS: discard
    return;
  }
  recording = on; adpcm = AdpcmState(); frameSeq = 0;
  if (!on) {
    micStop();
    lastSpeechEndMs = millis();
    requestConnParams(false);
  }
  led(LED_RED, false);
  Serial.printf("recording %s\n", on ? "ON" : "OFF");
  publishState();
}
void pumpAudio() {
  if (!micOn) return;
  if (pcmWrite - pcmRead > RING_SAMPLES - FRAME_SAMPLES) {
    // ring overrun (BLE stalled): drop the oldest frames; seq gap tells the receiver
    uint32_t drop = ((pcmWrite - pcmRead) - (RING_SAMPLES - FRAME_SAMPLES) + FRAME_SAMPLES - 1) / FRAME_SAMPLES;
    pcmRead += drop * FRAME_SAMPLES;
    if (recording) frameSeq += drop;
  }
  if (!recording) return;    // pre-roll: keep buffering until the press counts as a talk
  while (pcmWrite - pcmRead >= FRAME_SAMPLES) {
    uint8_t pkt[5 + FRAME_SAMPLES / 2];
    pkt[0] = frameSeq & 0xFF; pkt[1] = frameSeq >> 8;
    pkt[2] = adpcm.pred & 0xFF; pkt[3] = (uint16_t)adpcm.pred >> 8; pkt[4] = adpcm.index;
    for (uint32_t i = 0; i < FRAME_SAMPLES; i += 2) {
      uint8_t lo = adpcmEncodeSample(pcmBuf[(pcmRead + i) % RING_SAMPLES]);
      uint8_t hi = adpcmEncodeSample(pcmBuf[(pcmRead + i + 1) % RING_SAMPLES]);
      pkt[5 + i / 2] = lo | (hi << 4);
    }
    pcmRead += FRAME_SAMPLES; frameSeq++;
    if (Bluefruit.connected() && audioChr.notifyEnabled()) audioChr.notify(pkt, sizeof(pkt));
  }
}

void sendAlert(uint8_t level, int16_t ppm) {
  uint8_t a[3] = {level, (uint8_t)ppm, (uint8_t)(ppm >> 8)};
  alertChr.write(a, sizeof(a));
  if (Bluefruit.connected()) alertChr.notify(a, sizeof(a));
  Serial.printf("ALERT level %u (%d ppm) -> phone\n", level, ppm);
}
bool talkHeld() { return digitalRead(PIN_TALK) == LOW; }
// Returns false if the burst was skipped or aborted (button pressed); caller retries later.
bool measureCo2() {
  int32_t ppmSum = 0; float tSum = 0, rhSum = 0; uint8_t kept = 0;
  if (co2.exitSleepMode()) { Serial.println("STCC4 wake error"); }
  for (uint8_t i = 0; i < CO2_SHOTS; i++) {
    if (talkHeld()) { co2.enterSleepMode(); return false; }   // each shot blocks 500 ms; don't delay a talk
    int16_t ppm = 0; float t = 0, rh = 0; uint16_t status = 0;
    if (co2.measureSingleShot() || co2.readMeasurement(ppm, t, rh, status)) {
      Serial.println("STCC4 read error"); continue;
    }
    if (i == 0) continue;                                      // first shot after sleep is the noisiest
    ppmSum += ppm; tSum += t; rhSum += rh; kept++;
  }
  co2.enterSleepMode();
  if (!kept) return true;                                      // sensor error: wait a full period
  int16_t ppm = (int16_t)(ppmSum / kept); float t = tSum / kept, rh = rhSum / kept;
  bool exhale = recording || (lastSpeechEndMs && millis() - lastSpeechEndMs < EXHALE_HOLDOFF_MS);
  uint8_t level = ppm >= CO2_ALERT_PPM ? 2 : (ppm >= CO2_WARN_PPM ? 1 : 0);
  uint8_t flags = (exhale ? F_EXHALE : 0) | (level >= 1 ? F_WARN : 0) | (level >= 2 ? F_ALERT : 0);
  if (!exhale && level && (lastAlertMs[level] == 0 || millis() - lastAlertMs[level] > ALERT_COOLDOWN_MS)) {
    lastAlertMs[level] = millis(); flags |= F_NEWALERT; sendAlert(level, ppm);
  }
  Serial.printf("CO2 %d ppm  T %.1f C  RH %.1f %%  flags %02x\n", ppm, t, rh, flags);
  int16_t t100 = (int16_t)(t * 100); uint16_t rh100 = (uint16_t)(rh * 100);
  uint8_t pkt[7] = {(uint8_t)ppm, (uint8_t)(ppm >> 8), (uint8_t)t100, (uint8_t)(t100 >> 8),
                    (uint8_t)rh100, (uint8_t)(rh100 >> 8), flags};
  envChr.write(pkt, sizeof(pkt));
  if (Bluefruit.connected()) envChr.notify(pkt, sizeof(pkt));
  publishState();
  return true;
}
void serviceCo2() {
  if ((int32_t)(millis() - nextCo2Ms) < 0) return;
  bool busy = micOn || recording || (lastSpeechEndMs && millis() - lastSpeechEndMs < EXHALE_HOLDOFF_MS);
  if (busy || !measureCo2()) { nextCo2Ms = millis() + CO2_RETRY_MS; return; }
  nextCo2Ms = millis() + CO2_PERIOD_MS;
}

void onCtrl(uint16_t, BLECharacteristic*, uint8_t* data, uint16_t len) {
  if (!len) return;
  if (data[0] == 0x00) setRecording(false);
#if CTRL_REMOTE_START
  else if (data[0] == 0x01) { micStart(); setRecording(true); }
#endif
}
void onConnect(uint16_t conn) {
  connHandle = conn;
  BLEConnection* c = Bluefruit.Connection(conn);
  c->requestPHY();                 // 2M
  c->requestDataLengthUpdate();
  c->requestMtuExchange(247);      // 165 B audio packets need MTU >= 168
  requestConnParams(micOn);
}
void onDisconnect(uint16_t, uint8_t) { connHandle = BLE_CONN_HANDLE_INVALID; setRecording(false); }

void setupBLE() {
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);        // MTU 247 for 165 B audio packets
  Bluefruit.begin();
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);       // XIAO nRF52840 has the REG1 inductor (Zephyr xiao_ble dts)
  Bluefruit.setTxPower(0);
  Bluefruit.setName("Agent Pendant");
  Bluefruit.autoConnLed(false);
  Bluefruit.Security.setIOCaps(true, false, false);    // "display only": phone user types the static passkey
  Bluefruit.Security.setPIN(PAIR_PIN);
  Bluefruit.Periph.setConnectCallback(onConnect);
  Bluefruit.Periph.setDisconnectCallback(onDisconnect);
  Bluefruit.Periph.setConnInterval(IDLE_MIN_INT, IDLE_MAX_INT);
  svc = BLEService(uuid(0x01)); svc.begin();
  // Every characteristic needs an encrypted, MITM-protected (passkey) link: no unpaired listener or controller.
  audioChr = BLECharacteristic(uuid(0x02)); audioChr.setProperties(CHR_PROPS_NOTIFY);
  audioChr.setPermission(SECMODE_ENC_WITH_MITM, SECMODE_NO_ACCESS); audioChr.setMaxLen(5 + FRAME_SAMPLES / 2); audioChr.begin();
  envChr = BLECharacteristic(uuid(0x03)); envChr.setProperties(CHR_PROPS_NOTIFY | CHR_PROPS_READ);
  envChr.setPermission(SECMODE_ENC_WITH_MITM, SECMODE_NO_ACCESS); envChr.setFixedLen(7); envChr.begin();
  ctrlChr = BLECharacteristic(uuid(0x04)); ctrlChr.setProperties(CHR_PROPS_WRITE | CHR_PROPS_WRITE_WO_RESP);
  ctrlChr.setPermission(SECMODE_NO_ACCESS, SECMODE_ENC_WITH_MITM); ctrlChr.setMaxLen(4); ctrlChr.setWriteCallback(onCtrl); ctrlChr.begin();
  stateChr = BLECharacteristic(uuid(0x05)); stateChr.setProperties(CHR_PROPS_NOTIFY | CHR_PROPS_READ);
  stateChr.setPermission(SECMODE_ENC_WITH_MITM, SECMODE_NO_ACCESS); stateChr.setFixedLen(4); stateChr.begin();
  alertChr = BLECharacteristic(uuid(0x06)); alertChr.setProperties(CHR_PROPS_NOTIFY | CHR_PROPS_READ);
  alertChr.setPermission(SECMODE_ENC_WITH_MITM, SECMODE_NO_ACCESS); alertChr.setFixedLen(3); alertChr.begin();
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addService(svc);
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(320, 1600);         // 200 ms fast, then 1 s slow
  Bluefruit.Advertising.setFastTimeout(30);
  Bluefruit.Advertising.start(0);
}

// The privacy switch shorts the pin to GND while ON; a permanent 13 k pull-up would then draw ~250 uA.
// Enable the pull-up only for the instant of the read.
bool readPrivacy() {
  pinMode(PIN_PRIVACY, INPUT_PULLUP);
  delayMicroseconds(20);
  bool on = digitalRead(PIN_PRIVACY) == LOW;
  pinMode(PIN_PRIVACY, INPUT_PULLDOWN);   // no current through the closed switch; pin parked low
  return on;
}
void serviceInputs() {
  static uint32_t lastPrivacy = 0;
  if (millis() - lastPrivacy >= PRIVACY_POLL_MS) {
    lastPrivacy = millis();
    bool p = readPrivacy();
    if (p != privacy) {
      privacy = p; if (privacy) { setRecording(false); micStop(); micPowerOff(); }
      Serial.printf("privacy %s\n", privacy ? "ON" : "OFF"); publishState();
    }
  }
  static bool held = false; static uint32_t since = 0;
  bool r = talkHeld();
  if (r != held) {
    held = r; since = millis();
    if (held) micStart();                                   // start buffering immediately (pre-roll)
  }
  if (held && micOn && !recording && millis() - since >= TALK_MIN_MS) setRecording(true);
  if (!held && micOn && millis() - since >= 40) setRecording(false);  // also discards a too-short tap
}
void serviceLed() {
  static uint32_t last = 0;
  if (recording) { led(LED_RED, (millis() % 500) < 100); led(LED_GREEN, false); return; }  // 20 % duty blink
  led(LED_RED, false);
  bool adv = !Bluefruit.connected() && !privacy;
  if (adv && millis() - last >= 4000) last = millis();
  led(LED_GREEN, adv && millis() - last < 30);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_RED, OUTPUT); pinMode(LED_GREEN, OUTPUT); pinMode(LED_BLUE, OUTPUT);
  led(LED_RED, false); led(LED_GREEN, false); led(LED_BLUE, false);
  pinMode(PIN_TALK, INPUT_PULLUP);
  pinMode(VBAT_ENABLE, OUTPUT); digitalWrite(VBAT_ENABLE, LOW);   // keep LOW (protects P0.31 while charging)
  // P0.13 (charge current select) is left untouched: LOW would raise charging to 100 mA, too much for <100 mAh cells.
  micPowerOff();
  Wire.begin();
  co2.begin(Wire, STCC4_I2C_ADDR_64);
  delay(10);
  co2.exitSleepMode(); co2.stopContinuousMeasurement();
  // After power-up (>3 h unpowered counts as "idle") the STCC4 needs conditioning to reach accuracy (~22 s, blocking).
  led(LED_BLUE, true);
  if (co2.performConditioning()) Serial.println("STCC4 conditioning error");
  led(LED_BLUE, false);
  co2.enterSleepMode();
  setupBLE();
  privacy = readPrivacy(); publishState();
  nextCo2Ms = millis() + 5000;
}
void loop() {
  serviceInputs();
  serviceLed();
  pumpAudio();
  serviceCo2();
  delay(micOn ? 2 : 50);   // FreeRTOS tickless idle; 50 ms idle polling is well inside the 150 ms press threshold
}
