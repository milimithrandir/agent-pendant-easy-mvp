# Voice to Grok Bot: iPhone MVP plan

## Goal and current status

**Hold the pendant button, speak, release, and have the recording reach Max's existing Grok Bot automatically.** No tap, share sheet, or Send action on the iPhone. Grok Bot's response appears in its phone and Mac app. The button is the only recording control; there is no always-listening mode.

The XIAO firmware already sends passkey-protected, 16 kHz IMA-ADPCM audio over a custom BLE service while the button is held. The only receiver today is `firmware/tools/pendant_rx.py`, which saves a WAV on a laptop. **There is no iPhone receiver or Grok Bot delivery integration yet. Nothing has been tested on hardware.**

This is a proposed integration, not a proven end-to-end build. In particular, an automatic routine is not known to be identical to sending a normal voice message in the existing chat.

## End-to-end path

```
hold button → pendant mic → encrypted BLE audio → iPhone receiver → WAV file
release button → iPhone sends audio automatically → Grok Bot → result in Bot app
```

Build a small **native iPhone app** using Core Bluetooth as the central. It pairs with the pendant, subscribes to AUDIO and STATE, decodes each 165-byte ADPCM frame using the existing Python receiver as a reference, fills sequence gaps with silence, and closes the take when recording ends. The app keeps a local queue until delivery is acknowledged. It should show connection, recording, sent, and failed states; it must never report a lost take as sent. CO₂ notifications and phone alerts can use the same BLE connection.

The app needs the `bluetooth-central` background mode and Core Bluetooth state restoration. Apple allows background BLE events, but it also limits background execution and restoration; a force-quit can prevent relaunch. **Locked-screen, phone-in-pocket operation is a gate to test on an actual iPhone**, including a long take, reconnect, and temporary network loss. A browser page is not the iPhone receiver for this custom BLE protocol.

## Delivery into Grok Bot

There is no documented endpoint here for a third-party app to inject an audio attachment as an ordinary user message into an existing Grok Bot chat. Test the supported routes before choosing one:

| Route | What it can do | What remains to verify |
|---|---|---|
| **Bot email + email-triggered routine (audio-first candidate)** | The iPhone app sends the WAV as an email attachment to the Bot's own address. A routine triggered by that email asks the Bot to read/transcribe the attachment and handle the spoken request. This needs an authenticated mail-sending service or a small relay; the iPhone Mail compose sheet would require a tap and does not meet the goal. | Whether the routine can read the attachment, act on it without another prompt, report in the desired Bot conversation, and do so quickly enough. Grok Bot treats email contents as untrusted input, so its approval rules still apply. Claiming a Bot address requires setup. |
| **Webhook routine (automatic fallback)** | The iPhone app sends JSON to the Bot's authenticated webhook. The documented body can contain a transcript; the Bot receives it with the routine instruction and posts the run result in its chat. | Speech-to-text must happen on the iPhone or through a separate service. A webhook does not document direct audio attachment ingestion. Test conversation continuity, approvals, and latency. If audio must also be available, test a short-lived, access-controlled download URL in the JSON. |
| **iPhone share sheet** | Places an audio file in a Grok Bot conversation's composer. | Requires choosing the chat and sending manually, so it fails the automatic-delivery requirement. Use only to test how the Bot handles a WAV. |
| **Grok Bot iOS “Send Message” Shortcut** | The app exposes this Shortcut. | Its docs do not establish that another app can pass in a WAV and send unattended. Test separately before using it in the architecture. |

**Recommended first experiment:** give Grok Bot a short WAV by attachment, then create an email-triggered routine and send the same WAV to its Bot address. Verify that the Bot transcribes it, follows the spoken instruction, and reports back without a second user action. If that fails the intended chat behavior, test the webhook with a transcript. Do not commit to a mail service, speech-to-text provider, or phone app architecture beyond BLE reception until this route is proven.

Do not put a Bot email address, webhook key, mail credentials, or voice recordings in this repository. Keep delivery credentials out of the firmware. Any iPhone-to-relay credentials should be scoped and revocable.

## Build order and acceptance checks

1. **No hardware:** record a WAV on the phone; manually attach it in Grok Bot to check voice understanding. Set up and test the email-triggered routine with that WAV. Record where the result appears and whether actions need approval.
2. **Pendant + laptop:** flash and pair the XIAO, press/release, and confirm that `pendant_rx.py` creates a complete WAV. Measure first/last syllable loss and dropped frames.
3. **iPhone BLE receiver:** decode and save the same WAV on an unlocked and locked iPhone. Test while Grok Bot is closed, after reconnect, and after the iPhone receiver has been force-quit; document failures honestly.
4. **Automatic delivery:** release the button and verify that exactly one take reaches the Bot, without touching the phone. Test offline queue/retry, duplicate suppression, and an expired credential. A successful HTTP/email handoff is not yet proof that the Bot completed the request.
5. **Wear test:** measure real battery life, BLE reliability with phone in pocket, audio quality, CO₂ behavior, delivery latency, and Bot usage before designing v2.

The first useful milestone is **one complete spoken request from the pendant to Grok Bot with the phone locked and no phone interaction**. Anything short of that is a component test.

## Sources and current limits

- [Grok Bot: files and audio attachments](https://docs.x.ai/grok-bot/files-and-results)
- [Grok Bot: own email address and email trust model](https://docs.x.ai/grok-bot/computer-and-apps)
- [Grok Bot: email and webhook routines](https://docs.x.ai/grok-bot/skills-routines-and-automations)
- [Grok Bot on iPhone: share sheet and Shortcuts](https://docs.x.ai/grok-bot/mobile)
- [Apple: Core Bluetooth background processing](https://developer.apple.com/library/archive/documentation/NetworkingInternetWeb/Conceptual/CoreBluetooth_concepts/CoreBluetoothBackgroundProcessingForIOSApps/PerformingTasksWhileYourAppIsInTheBackground.html)
- [Apple: Bluetooth state restoration relaunch rules](https://developer.apple.com/documentation/technotes/tn3115-bluetooth-state-restoration-app-relaunch-rules)
