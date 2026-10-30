#!/usr/bin/env python3
"""Laptop-side test receiver for Agent Pendant v0.
pip install bleak ; python pendant_rx.py
v0: press button to record, press again to stop. v0_nohaptic / v0_slim_nrf52: HOLD to talk, release to stop.
Each take -> rec_<n>.wav (dropped frames are filled with silence). CO2 readings print every 60 s (v0) or
5 min (no-haptics firmware); CO2 ALERT notifications are printed and raised as a desktop notification if
notify-send/osascript exists (stand-in for the phone app).
No-haptics firmware requires pairing: enter the passkey (PAIR_PIN in the sketch, default 472913) when the OS asks."""
import asyncio, struct, wave, time, shutil, subprocess, sys
from bleak import BleakScanner, BleakClient

AUDIO = "6e400002-a9e4-4c1b-9d3e-ae9e0a9e0001"
ENV   = "6e400003-a9e4-4c1b-9d3e-ae9e0a9e0001"
STATE = "6e400005-a9e4-4c1b-9d3e-ae9e0a9e0001"   # [recording u8][privacy u8][batt_mV u16]  (nohaptic fw)
ALERT = "6e400006-a9e4-4c1b-9d3e-ae9e0a9e0001"   # [level u8][ppm i16] level 1 >=1000, 2 >=1500 (nohaptic fw)
IDX = [-1,-1,-1,-1,2,4,6,8]*2
STEP = [7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,50,55,60,66,73,80,88,97,107,118,130,143,157,173,190,209,230,253,279,307,337,371,408,449,494,544,598,658,724,796,876,963,1060,1166,1282,1411,1552,1707,1878,2066,2272,2499,2749,3024,3327,3660,4026,4428,4871,5358,5894,6484,7132,7845,8630,9493,10442,11487,12635,13899,15289,16818,18500,20350,22385,24623,27086,29794,32767]

def decode(pred, index, data):
    out = bytearray()
    for byte in data:
        for code in (byte & 0x0F, byte >> 4):
            step = STEP[index]; delta = step >> 3
            if code & 4: delta += step
            if code & 2: delta += step >> 1
            if code & 1: delta += step >> 2
            pred = max(-32768, min(32767, pred - delta if code & 8 else pred + delta))
            index = max(0, min(88, index + IDX[code]))
            out += struct.pack("<h", pred)
    return out

class Rec:
    def __init__(self): self.pcm = bytearray(); self.last = 0; self.n = 0; self.seq = None
    def audio(self, _, d):
        seq, pred, idx = struct.unpack_from("<HhB", d)
        if seq == 0 and self.pcm: self.flush()
        if self.seq is not None and seq > self.seq + 1:   # firmware dropped frames (BLE stall): keep timing
            gap = seq - self.seq - 1
            print(f"  {gap} frame(s) lost"); self.pcm += bytes(640 * gap)
        self.seq = seq
        self.pcm += decode(pred, idx, d[5:]); self.last = time.time()
    def flush(self):
        if not self.pcm: return
        name = f"rec_{self.n}.wav"; self.n += 1
        with wave.open(name, "wb") as w:
            w.setnchannels(1); w.setsampwidth(2); w.setframerate(16000); w.writeframes(self.pcm)
        print(f"saved {name} ({len(self.pcm)/32000:.1f} s)"); self.pcm = bytearray(); self.seq = None

def env(_, d):
    ppm, t, rh, fl = struct.unpack("<hhHB", d)
    print(f"CO2 {ppm} ppm  T {t/100:.1f}C  RH {rh/100:.1f}%  flags {fl:#04x}")

def notify(title, msg):
    if sys.platform == "darwin" and shutil.which("osascript"):
        subprocess.run(["osascript", "-e", f'display notification "{msg}" with title "{title}"'])
    elif shutil.which("notify-send"):
        subprocess.run(["notify-send", title, msg])

def alert(_, d):
    level, ppm = struct.unpack("<Bh", d)
    msg = f"CO2 {ppm} ppm - " + ("ventilate now" if level >= 2 else "getting stuffy")
    print(f"*** ALERT level {level}: {msg}"); notify("Agent Pendant", msg)

def make_state(r):
    def state(_, d):
        rec, priv, mv = struct.unpack("<BBH", d)
        print(f"state: recording={rec} privacy={priv} batt={mv} mV")
        if not rec: r.flush()
    return state

async def main():
    dev = await BleakScanner.find_device_by_name("Agent Pendant", timeout=20)
    if not dev: raise SystemExit("pendant not found")
    r = Rec()
    async with BleakClient(dev) as c:
        try: await c.pair()      # Linux/Windows; on macOS the OS prompts for the passkey on first access
        except Exception: pass
        await c.start_notify(AUDIO, r.audio); await c.start_notify(ENV, env)
        for uuid, cb in ((STATE, make_state(r)), (ALERT, alert)):
            try: await c.start_notify(uuid, cb)
            except Exception: pass   # older v0 firmware has no STATE/ALERT
        print("connected; hold the button to talk")
        while True:
            await asyncio.sleep(0.5)
            if r.pcm and time.time() - r.last > 1.0: r.flush()

asyncio.run(main())
