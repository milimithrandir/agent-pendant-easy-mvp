// Agent Pendant v0 - NO-HAPTICS firmware
// Board: Seeed XIAO ESP32S3 (Arduino-ESP32 core 3.x, FQBN esp32:esp32:XIAO_ESP32S3)
// Parts: Adafruit STCC4+SHT41 (I2C 0x64), Adafruit 3492 PDM mic (VDD through the privacy slide switch),
//        hold-to-talk = 6 mm tact button (default) or capacitive touch pad (#define HTT_TOUCH 1),
//        onboard user LED (GPIO21) as status LED / light pipe. No DRV2605L, no motor.
// Libraries: "Sensirion I2C STCC4", "NimBLE-Arduino" (2.x)
//
// Behaviour
//  - HOLD TO TALK: audio streams only while the button/pad is held; stops on release (min 150 ms press).
//  - Privacy switch (D2 = GPIO3 to GND): ON = mic hardware-cut (switch breaks mic VDD) and firmware ignores talk.
//  - LED: solid while recording; 30 ms blip every 4 s while advertising (not connected); off otherwise / in privacy.
//  - CO2/T/RH: burst of 3 single shots every 5 min (first discarded, mean of the rest) -> ENV notify. Never measured
//    while talking or within 120 s after (exhaled breath, and each 500 ms blocking shot would stall audio).
//  - Threshold alerts are NOT local any more: ALERT characteristic notifies the phone (level 1 = >=1000 ppm,
//    level 2 = >=1500 ppm), rate-limited per level; phone app shows the notification.
//  - BLE: passkey pairing (PAIR_PIN) required before any data is sent or CTRL is accepted.
//  - Phone can write CTRL 0x00 (stop); 0x01 (start) only with -DCTRL_REMOTE_START=1 (debug), ignored in privacy.

#include <Arduino.h>
#include <Wire.h>
#include <ESP_I2S.h>
#include <SensirionI2cStcc4.h>
#include <NimBLEDevice.h>

#ifndef PAIR_PIN
#define PAIR_PIN 472913      // 6-digit static passkey typed on the phone when pairing; change per device
#endif
#ifndef CTRL_REMOTE_START
#define CTRL_REMOTE_START 0  // 1 = allow CTRL 0x01 to start recording from the phone (debug only)
#endif
#ifndef HTT_TOUCH
#define HTT_TOUCH 0          // 0 = tact button on D1 to GND, 1 = touch electrode on D1 (GPIO2 = TOUCH2)
#endif

// ---------- Pins (XIAO ESP32S3) ----------
#define PIN_SDA      5   // D4
#define PIN_SCL      6   // D5
#define PIN_TALK     2   // D1: button to GND, or touch electrode (copper foil behind the front face)
#define PIN_PRIVACY  3   // D2: privacy slide switch 2nd pole to GND (LOW = privacy ON)
#define PIN_MIC_CLK  7   // D8 -> mic CLK
#define PIN_MIC_DAT  8   // D9 -> mic DAT (SEL to GND = left)
#define PIN_LED      21  // onboard user LED, active LOW

// ---------- Tunables ----------
static const uint32_t SAMPLE_RATE       = 16000;
static const uint32_t FRAME_SAMPLES     = 320;     // 20 ms
static const uint32_t CO2_PERIOD_MS     = 5UL * 60UL * 1000UL;
static const uint8_t  CO2_SHOTS         = 3;       // first shot discarded, the rest averaged
static const uint32_t EXHALE_HOLDOFF_MS = 120000;  // STCC4 tau63 is 20 s; ~95 % recovery ~60 s plus case delay
static const uint32_t CO2_RETRY_MS      = 10000;
static const int16_t  CO2_WARN_PPM      = 1000;
static const int16_t  CO2_ALERT_PPM     = 1500;
static const uint32_t ALERT_COOLDOWN_MS = 10UL * 60UL * 1000UL;
static const uint32_t TALK_MIN_MS       = 150;     // ignore taps shorter than this (false triggers)
static const float    TOUCH_RATIO       = 1.25f;   // touched if reading > baseline * ratio (ESP32-S3 values rise on touch)

// ---------- BLE UUIDs (same service as v0, + STATE and ALERT) ----------
#define SVC_UUID    "6e400001-a9e4-4c1b-9d3e-ae9e0a9e0001"
#define AUDIO_UUID  "6e400002-a9e4-4c1b-9d3e-ae9e0a9e0001"  // notify: ADPCM frames
#define ENV_UUID    "6e400003-a9e4-4c1b-9d3e-ae9e0a9e0001"  // notify/read: CO2,T,RH,flags
#define CTRL_UUID   "6e400004-a9e4-4c1b-9d3e-ae9e0a9e0001"  // write: 0x01 start, 0x00 stop (debug)
#define STATE_UUID  "6e400005-a9e4-4c1b-9d3e-ae9e0a9e0001"  // notify/read: [recording u8][privacy u8][batt_mV u16]
#define ALERT_UUID  "6e400006-a9e4-4c1b-9d3e-ae9e0a9e0001"  // notify/read: [level u8][ppm i16]

// ENV flags
#define F_EXHALE 0x01
#define F_WARN   0x02
#define F_ALERT  0x04
#define F_NEWALERT 0x08   // this sample raised an ALERT notification

I2SClass i2s;
SensirionI2cStcc4 co2;
NimBLECharacteristic *audioChr, *envChr, *stateChr, *alertChr;

volatile bool bleConnected = false;
volatile bool linkSecure = false;      // encrypted + passkey-authenticated
bool recording = false, privacy = false;
uint32_t lastSpeechEndMs = 0, nextCo2Ms = 0, lastAlertMs[3] = {0, 0, 0};
uint16_t frameSeq = 0;
uint32_t touchBase = 0;

// ---------- IMA ADPCM encoder ----------
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
  int diff = s - adpcm.pred;
  uint8_t code = 0;
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

// ---------- BLE ----------
void setRecording(bool on);
void publishState();
void requestTalkParams(bool talk);
class ServerCB : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* s, NimBLEConnInfo& ci) override {
    bleConnected = true; linkSecure = false;
    NimBLEDevice::startSecurity(ci.getConnHandle());              // pair/bond before any data flows
    s->updateConnParams(ci.getConnHandle(), 36, 48, 30, 600);    // idle: 45-60 ms, latency 30, 6 s timeout
  }
  void onDisconnect(NimBLEServer*, NimBLEConnInfo&, int) override {
    bleConnected = false; linkSecure = false; setRecording(false); NimBLEDevice::startAdvertising();
  }
  uint32_t onPassKeyDisplay() override { return PAIR_PIN; }
  void onAuthenticationComplete(NimBLEConnInfo& ci) override {
    linkSecure = ci.isEncrypted() && ci.isAuthenticated();
    if (!linkSecure) NimBLEDevice::getServer()->disconnect(ci.getConnHandle());
  }
};
class CtrlCB : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* c, NimBLEConnInfo&) override {
    std::string v = c->getValue();
    if (v.empty() || !linkSecure) return;
    if (v[0] == 0x00) setRecording(false);
#if CTRL_REMOTE_START
    else if (v[0] == 0x01) setRecording(true);
#endif
  }
};
void setupBLE() {
  NimBLEDevice::init("Agent Pendant");
  NimBLEDevice::setMTU(247);
  NimBLEDevice::setSecurityAuth(true, true, true);              // bonding, MITM, LE Secure Connections
  NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);        // phone user types PAIR_PIN
  NimBLEDevice::setSecurityPasskey(PAIR_PIN);
  NimBLEServer* srv = NimBLEDevice::createServer();
  srv->setCallbacks(new ServerCB());
  NimBLEService* svc = srv->createService(SVC_UUID);
  audioChr = svc->createCharacteristic(AUDIO_UUID, NIMBLE_PROPERTY::NOTIFY);
  envChr   = svc->createCharacteristic(ENV_UUID, NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ);
  stateChr = svc->createCharacteristic(STATE_UUID, NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ);
  alertChr = svc->createCharacteristic(ALERT_UUID, NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ);
  NimBLECharacteristic* ctrl = svc->createCharacteristic(CTRL_UUID, NIMBLE_PROPERTY::WRITE);
  ctrl->setCallbacks(new CtrlCB());
  svc->start();
  NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
  adv->addServiceUUID(SVC_UUID);
  adv->setName("Agent Pendant");
  adv->start();
}
uint16_t batteryMv() {
  // XIAO ESP32S3 has no onboard battery divider; returns 0 unless you add one (2x 200k to an ADC pin).
  return 0;
}
void publishState() {
  uint16_t mv = batteryMv();
  uint8_t s[4] = {(uint8_t)recording, (uint8_t)privacy, (uint8_t)mv, (uint8_t)(mv >> 8)};
  stateChr->setValue(s, sizeof(s));
  if (linkSecure) stateChr->notify();
}

// ---------- Recording ----------
void setRecording(bool on) {
  if (on && privacy) on = false;
  if (on == recording) return;
  recording = on;
  adpcm = AdpcmState(); frameSeq = 0;
  requestTalkParams(on);
  digitalWrite(PIN_LED, on ? LOW : HIGH);
  if (!on) lastSpeechEndMs = millis();
  Serial.printf("recording %s\n", on ? "ON" : "OFF");
  publishState();
}
void pumpAudio() {
  static int16_t pcm[FRAME_SAMPLES];
  size_t got = i2s.readBytes((char*)pcm, sizeof(pcm));
  if (!recording || got < sizeof(pcm)) return;
  uint8_t pkt[5 + FRAME_SAMPLES / 2];
  pkt[0] = frameSeq & 0xFF; pkt[1] = frameSeq >> 8;
  pkt[2] = adpcm.pred & 0xFF; pkt[3] = (uint16_t)adpcm.pred >> 8;
  pkt[4] = adpcm.index;
  for (uint32_t i = 0; i < FRAME_SAMPLES; i += 2) {
    uint8_t lo = adpcmEncodeSample(pcm[i]);
    uint8_t hi = adpcmEncodeSample(pcm[i + 1]);
    pkt[5 + i / 2] = lo | (hi << 4);
  }
  frameSeq++;
  if (linkSecure) audioChr->notify(pkt, sizeof(pkt));
}

// ---------- CO2 + phone alerts ----------
void sendAlert(uint8_t level, int16_t ppm) {
  uint8_t a[3] = {level, (uint8_t)ppm, (uint8_t)(ppm >> 8)};
  alertChr->setValue(a, sizeof(a));
  if (linkSecure) alertChr->notify();
  Serial.printf("ALERT level %u (%d ppm) -> phone\n", level, ppm);
}
bool talkRaw();
// Returns false if the burst was aborted (talk pressed); caller retries later.
bool measureCo2() {
  int32_t ppmSum = 0; float tSum = 0, rhSum = 0; uint8_t kept = 0;
  co2.exitSleepMode();
  for (uint8_t i = 0; i < CO2_SHOTS; i++) {
    if (talkRaw()) { co2.enterSleepMode(); return false; }    // each shot blocks 500 ms; don't delay a talk
    int16_t p = 0; float tt = 0, h = 0; uint16_t status = 0;
    if (co2.measureSingleShot() || co2.readMeasurement(p, tt, h, status)) {
      Serial.println("STCC4 read error"); continue;
    }
    if (i == 0) continue;                                     // first shot after sleep is the noisiest
    ppmSum += p; tSum += tt; rhSum += h; kept++;
  }
  co2.enterSleepMode();
  if (!kept) return true;
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
  envChr->setValue(pkt, sizeof(pkt));
  if (linkSecure) envChr->notify();
  return true;
}
void serviceCo2() {
  if ((int32_t)(millis() - nextCo2Ms) < 0) return;
  bool busy = recording || (lastSpeechEndMs && millis() - lastSpeechEndMs < EXHALE_HOLDOFF_MS);
  if (busy || !measureCo2()) { nextCo2Ms = millis() + CO2_RETRY_MS; return; }
  nextCo2Ms = millis() + CO2_PERIOD_MS;
}

// ---------- Inputs ----------
bool talkRaw() {
#if HTT_TOUCH
  return touchRead(PIN_TALK) > (uint32_t)(touchBase * TOUCH_RATIO);
#else
  return digitalRead(PIN_TALK) == LOW;
#endif
}
void serviceInputs() {
  // privacy switch (debounced by polling rate)
  bool p = digitalRead(PIN_PRIVACY) == LOW;
  if (p != privacy) {
    privacy = p; if (privacy) setRecording(false);
    Serial.printf("privacy %s\n", privacy ? "ON" : "OFF"); publishState();
  }
  // hold-to-talk with debounce + minimum hold time
  static bool held = false; static uint32_t since = 0;
  bool r = talkRaw();
  if (r != held) { held = r; since = millis(); }
  if (held && !recording && millis() - since >= TALK_MIN_MS) setRecording(true);
  if (!held && recording && millis() - since >= 40) setRecording(false);
}
void requestTalkParams(bool talk) {
  NimBLEServer* s = NimBLEDevice::getServer();
  if (!bleConnected || !s->getConnectedCount()) return;
  uint16_t h = s->getPeerInfo(0).getConnHandle();
  if (talk) s->updateConnParams(h, 12, 24, 0, 400);            // 15-30 ms while streaming
  else      s->updateConnParams(h, 36, 48, 30, 600);
}
void serviceLed() {
  if (recording) return;                     // solid on handled in setRecording
  static uint32_t last = 0;
  bool blip = !bleConnected && !privacy && (millis() - last) >= 4000;
  if (blip) { last = millis(); }
  digitalWrite(PIN_LED, (!bleConnected && !privacy && millis() - last < 30) ? LOW : HIGH);
}

// ---------- Main ----------
void setup() {
  Serial.begin(115200);
  pinMode(PIN_PRIVACY, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT); digitalWrite(PIN_LED, HIGH);
#if HTT_TOUCH
  uint64_t acc = 0; for (int i = 0; i < 32; i++) { acc += touchRead(PIN_TALK); delay(5); }
  touchBase = acc / 32;                       // calibrate untouched baseline at boot (do not touch while booting)
  Serial.printf("touch baseline %lu\n", (unsigned long)touchBase);
#else
  pinMode(PIN_TALK, INPUT_PULLUP);
#endif
  Wire.begin(PIN_SDA, PIN_SCL);
  co2.begin(Wire, STCC4_I2C_ADDR_64);
  delay(10);
  co2.exitSleepMode(); co2.stopContinuousMeasurement();
  // After power-up the STCC4 needs conditioning to reach accuracy (~22 s, blocking).
  if (co2.performConditioning()) Serial.println("STCC4 conditioning error");
  co2.enterSleepMode();
  i2s.setPinsPdmRx(PIN_MIC_CLK, PIN_MIC_DAT);
  if (!i2s.begin(I2S_MODE_PDM_RX, SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO))
    Serial.println("PDM mic init failed");
  setupBLE();
  privacy = digitalRead(PIN_PRIVACY) == LOW; publishState();
  nextCo2Ms = millis() + 5000;
}
void loop() {
  serviceInputs();
  serviceLed();
  pumpAudio();                                // blocks ~20 ms per frame, paces the loop
  serviceCo2();
}
