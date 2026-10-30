# Voice to Grok Bot: the bridge plan

**Goal (2026-10-30):** hold the button, talk, release, and the message lands in Max's Grok Bot automatically. No typing, no tapping in an app. **Status: plan only, nothing built yet.**

## The idea in one line
Grok Bot **routines can be started by a webhook**: a private URL plus a secret key. Anything POSTed to it starts the Bot with that message, and the Bot's reply shows up in the normal Grok Bot chat (Mac app and phone app, with push notifications). Source: [Grok Bot docs, Skills and routines → Webhooks](https://docs.x.ai/grok-bot/skills-routines-and-automations), read 2026-10-30.

## Flow
```
hold button → pendant streams ADPCM audio over BLE (protocol: firmware/README.md)
→ bridge on the Mac (later: phone) saves the take as a WAV
→ on-device speech-to-text (free): Whisper on the Mac, Apple / Android speech on the phone
→ POST {"text": "...", "time": "...", "co2_ppm": 812} to the Grok Bot webhook
→ the Bot handles it like a typed message and replies in its chat
```

## Why this route
| Route | Verdict |
|---|---|
| **Webhook routine** | **Chosen.** Official, documented, no UI automation. Text in, reply in the normal chat. |
| Paste into the Grok Bot app with keyboard automation (AppleScript) | Fragile hack. Breaks when the app's UI changes, Mac only. |
| Email the clip to the Bot's own address | No code needed, but it's **unverified** whether the Bot handles audio attachments. Emails from routines wait as drafts. Keep as a fallback. |
| X Chat / Slack DM / tag @bot on X | Work, but they're clunkier than one POST, and tagging @bot on X is public. |

## Speech-to-text: free, on device
No paid API is needed. Only the text leaves the device.

| Where | Engine | Cost | Notes |
|---|---|---|---|
| **Mac (first)** | **Whisper** via `whisper.cpp` or `mlx-whisper` | free, offline | about 1 s per short clip on Apple silicon (est.); good at English and Finnish |
| iPhone | Apple on-device speech recognition | free | called from the small pendant app |
| Android | Android on-device speech recognizer, or Whisper | free | |
| Fallback | xAI speech-to-text (`api.x.ai/v1/stt`, `grok-voice-transcribe-2.0`) | paid xAI developer API key, price not checked | only if the free transcripts are poor |

## Grok Bot setup (about 2 minutes, once)
1. In the Grok Bot app, message the Bot you want to talk to:
   > Create a routine called "Pendant" with a webhook trigger. Each call's `text` field is a voice message from me, sent from my pendant. Handle it exactly as if I had typed it to you in this chat. `time` is when I said it and `co2_ppm` is the room CO2 at that moment; use them only if they matter.
2. On the Mac, open the Bot's details → **Routines** → **Pendant** → **Webhook**. Copy **POST to** (the URL) and **key** (sent as `Authorization: Bearer <key>`).
3. Use **Test** once, then check the reply lands in the Bot's chat.

A `200` response only means Grok Bot accepted the call and started a run, not that the Bot finished. The answer arrives in the chat.

## Bridge v1: Mac (`firmware/tools/grok_bridge.py`, to be written)
Extends `pendant_rx.py`, which already pairs, receives audio and saves `rec_N.wav`:
1. After each take: run Whisper on the WAV and get the text.
2. Skip empty or very short transcripts (accidental presses).
3. POST the JSON to the webhook. If it fails (no internet, non-200), save it to a local queue and retry.
4. Optionally forward CO2 alerts the same way, e.g. "CO2 is 1520 ppm", to a separate routine.

Config lives in environment variables, **never in the repo**:
```bash
export GROK_WEBHOOK_URL="https://..."     # "POST to" from the routine
export GROK_WEBHOOK_KEY="..."             # "key" from the routine
python firmware/tools/grok_bridge.py
```

## Bridge v2: phone (after the Mac works)
- **Android:** a page in Chrome using Web Bluetooth (seed: `step0/hold_to_talk.html`). Same protocol, on-device speech, same POST. Remember `requestMtu(247)`.
- **iPhone:** a small Swift app (CoreBluetooth background mode + on-device speech + POST). Safari has no Web Bluetooth, and only an app can stay connected to the pendant in the background.

## Costs and limits
- Speech-to-text: €0 (on device).
- Grok Bot: each voice message is **one routine run** against the weekly Grok Bot usage of Max's plan.
- Limits: 200 runs/hour per routine, 300/hour across all routines. Runs that fail with `Automation run rate limit exceeded` are not retried automatically, so the bridge's retry queue covers them.

## Security
- The webhook key works like a password: anyone who has it can send tasks to the Bot. Keep it in env vars or the Keychain; rotate it if it leaks.
- Grok Bot may act on what it hears. Keep its approval settings on for sending, buying and deleting.
- The pendant itself is protected by the BLE passkey (`PAIR_PIN`, see [`BUILD_GUIDE.md` §3](BUILD_GUIDE.md#3-flash-the-firmware)).

## Order of work
1. Create the Grok Bot routine and test it with `curl` (works today, before any hardware).
2. Write `grok_bridge.py` with Whisper; test with a WAV recorded on the Mac.
3. When the board arrives: pendant → Mac → Grok Bot, end to end.
4. Then the phone bridge (Android page or iPhone app).

Test call for step 1:
```bash
curl -X POST "$GROK_WEBHOOK_URL" \
  -H "Authorization: Bearer $GROK_WEBHOOK_KEY" \
  -H "Content-Type: application/json" \
  -d '{"text": "Test from the pendant bridge. Reply with OK."}'
```
