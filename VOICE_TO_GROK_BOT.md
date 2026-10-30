# Voice to Grok Bot: plan

**Goal:** hold the pendant button, talk, release, and what you said goes to your Grok Bot assistant automatically. The reply comes back to your phone.

**Status (2026-10-30):** plan only. Nothing is set up, nothing built, nothing tested on hardware. Each claim below is marked **verified** (checked in Grok Bot's docs or the firmware today) or **unverified**.

## The one decision for Max
**Which route should messages use to reach the assistant?** Recommended: **a webhook routine**, see below. Once you say yes, I create the routine. Grok Bot asks you to confirm it, then you copy its URL and key from the desktop app.

## 1. Routes into the assistant
| Route | How the phone app delivers | Status | Verdict |
|---|---|---|---|
| **Webhook routine** | HTTP `POST` to the routine's URL with `Authorization: Bearer <key>` and a JSON body such as `{"text":"…"}`. The assistant runs with that body plus the routine's instruction, and the result appears in the assistant's chat in the Grok Bot app (desktop and phone). | **Verified** in the [Grok Bot routines docs](https://docs.x.ai/grok-bot/skills-routines-and-automations): URL and key are shown under the routine's *Webhook* section on desktop; `200` = run started (not finished); limits are 200 runs/hour per routine and 300/hour across all routines; each run spends plan usage. | **Recommended.** No third-party account, works from any HTTP client, and the reply lands in the Grok Bot app you already use. |
| Assistant's own email inbox | The app emails the transcript to the assistant's address; an email-triggered routine wakes on it. | Trigger **verified** (docs). The assistant **has no address yet**: claiming one is permanent. A phone app would also need an email-sending service. | Works, but slower and more moving parts. Backup option. |
| Slack | The app posts to a Slack channel/DM with a Slack bot token; a Slack-triggered routine wakes on it and can reply in Slack. | Trigger **verified** (docs). Needs Slack connected on the Cursor account that owns the assistant, plus `/invite @Cursor` in that channel. Not set up. | Only worth it if you already live in Slack. |
| Telegram / WhatsApp / SMS / Discord | — | **No official channel** (third-party write-ups, Sept–Oct 2026; I also see no such channel among my own tools). Telegram would only work through a relay *you* run that forwards to the webhook. | Not recommended. |
| Tag @bot on X | — | Reported as not available in the EU (third-party write-up); unverified for Finland. | No. |
| Sending audio directly (no transcription) | Base64 audio inside the webhook JSON. | **Unverified**: body size limit unknown, and transcribing on the assistant's side would spend usage on every clip. | No. Transcribe first, send text. |

**Unverified, test in Stage 0:**
- **CORS:** whether a *browser page* is allowed to POST to the webhook URL. If not, use the laptop (Stage 1) or a tiny native Android wrapper, which isn't subject to CORS.
- Whether each webhook run continues the same conversation context as your normal chat, or starts fresh. The routine instruction can tell it to treat each run as a voice note from you.
- How fast a run starts and replies. Not measured. Expect seconds to tens of seconds, not instant.

## 2. Architecture
```
pendant (hold) --BLE, IMA-ADPCM 16 kHz--> phone page/app --decode to WAV--> speech-to-text
      --text--> POST webhook --> Grok Bot assistant --reply--> Grok Bot app on your phone
```
**What the firmware actually sends (verified from `agent_pendant_v0_slim_nrf52.ino` and `firmware/README.md`):**
- 16 kHz mono, 20 ms frames of 320 samples. Each BLE notify is 165 B: `[seq u16][pred i16][index u8]` + 160 B IMA-ADPCM, low nibble first. That's about 66 kbit/s.
- The ADPCM state travels in every packet, so a lost frame doesn't break decoding. Fill the gap with silence (like `pendant_rx.py`). `seq` restarts at 0 each take, which marks a new take.
- MTU must be ≥ 168. The pendant asks for 247 itself. Android Web Bluetooth has no MTU call, so whether Chrome accepts the pendant's request is **unverified on hardware**.
- Audio is encrypted, so it needs passkey pairing (your `PAIR_PIN`).
- Decoded audio is 32 kB/s, so a 10 s clip is about 320 kB WAV. That's well under the 25 MB upload limits.

**Speech-to-text options**
| Option | Cost | Latency | Notes |
|---|---|---|---|
| **Groq `whisper-large-v3-turbo`** | free tier: 2,000 requests/day, 8 h audio/day; paid $0.04/h, min 10 s billed per request | fast (216× real time per Groq) | **Recommended.** Takes WAV directly. Needs a free Groq API key. Audio goes to Groq. |
| OpenAI `gpt-transcribe` | $0.0045/min, no free tier | fast | `whisper-1` and `gpt-4o(-mini)-transcribe` are deprecated, removed 2027-02-26. |
| Local `whisper.cpp` on the laptop | free | about a few seconds per clip on a laptop (unmeasured) | Private. Good for Stage 1, not for the phone. |
| Web Speech API (Chrome) | free | live | **Only listens to the phone's own mic**, so it can't take the pendant's BLE audio. Fine for Stage 0 only. |

**Phones:** Android Chrome has Web Bluetooth, so a single https web page can do everything. **iPhone Safari has no Web Bluetooth**, so iPhone needs a native app (more work, and Apple's paid developer account for long-term installs) or the Bluefy browser for tests. Bluefy with passkey pairing is unverified.

**Keys:** the webhook key and the Groq key are passwords. They never go in this repo. The page asks for them once and keeps them only on the phone.

## 3. Staged plan
| Stage | What | Hardware | Effort (est.) | Cost |
|---|---|---|---|---|
| **0** | Create the webhook routine. Test it with one `curl` from the laptop. Then extend [`step0/hold_to_talk.html`](step0/hold_to_talk.html): hold → Web Speech text (or the recorded clip → Groq) → POST to webhook. This also settles CORS, latency and how the replies feel. | none | 1–2 h | €0 (plus Grok Bot usage per run) |
| **1** | Extend `tools/pendant_rx.py`: after each take's WAV is saved, send it to Groq (or `whisper.cpp`), then POST the text to the webhook. The laptop acts as the "phone". | easy-MVP build + laptop | ½ day | €0 on Groq free tier |
| **2** | Android web page: Web Bluetooth connect + pairing, ADPCM decode in JS (port of `decode()` in `pendant_rx.py`, ~20 lines), WAV → Groq → webhook. CO2 alerts are shown as phone notifications on the same page. | easy-MVP build + Android phone | 1–2 days | €0 (static https hosting, e.g. GitHub Pages) |
| 2-iOS | Only if your phone is an iPhone: a small native app doing the same. | + iPhone | several days (estimate) | Apple developer account if kept installed |

**Stage 0 test (after the routine exists; you paste your own URL and key):**
```bash
curl -X POST "$WEBHOOK_URL" -H "Authorization: Bearer $KEY" \
  -H "Content-Type: application/json" -d '{"text":"test from the pendant plan","source":"pendant"}'
# 200 = run started; the reply shows up in the assistant's chat in the Grok Bot app
```
**Draft routine instruction** (I'd save this only after you say yes): *"A webhook body is a voice note Max spoke into his pendant (`text` field, transcribed automatically, may contain transcription errors). Treat it as a message from Max and answer or act on it as you would in chat, keeping the same approval rules. Reply to Max in this chat."*

## Open unknowns
- CORS from a browser page to the webhook (Stage 0 tells us).
- Run start and reply latency, and whether runs share chat context.
- Grok Bot usage cost per voice note: each run spends plan usage. Not measured. Ten notes a day may matter on a small plan.
- Whether Chrome on Android accepts the pendant's MTU 247 and passkey pairing (Stage 2 on hardware).
- Whether Max's phone is Android or iPhone.
