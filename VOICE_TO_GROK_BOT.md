# Voice to Grok Bot: iPhone MVP plan

## Required experience

**Hold the pendant button, speak, release. Grok Bot receives the request and acts without any phone tap.** The iPhone may be locked; the Mac need not be on. There is no always-listening mode.

The current XIAO nRF52840 Sense firmware sends 16 kHz IMA-ADPCM audio over a custom, passkey-protected BLE service. The only receiver today is `firmware/tools/pendant_rx.py`, which saves WAV files on a laptop. There is no iPhone receiver yet, and no hardware test has happened. Max has confirmed that his Grok Bot can understand an audio attachment sent by email.

Two separate questions must be solved: **how the iPhone receives audio from this custom BLE device**, and **how Grok Bot accepts an unattended request**. Sending an ordinary voice message directly into its existing chat is a distinct outcome from starting a Bot routine; no documented third-party audio-message endpoint has been found.

## Check the direct app route first

Grok Bot exposes **Send Message** and **Start Voice Chat** actions in iPhone Shortcuts. Before building a relay, test the installed app:

1. In Shortcuts, add Grok Bot's **Send Message** action, select the intended Bot, and try a fixed text message. Does it send immediately, or open a composer/ask for confirmation?
2. Feed that action a recorded audio file as Shortcut input. Does it accept the file, send it as an audio attachment, and preserve the normal chat thread?
3. Test the Shortcut from the Lock Screen. Apple documents a `shortcuts://run-shortcut` URL with text input, so a future receiver app could attempt to invoke it; **whether it runs unattended from a background BLE event on a locked iPhone is unverified**. Do not design around it until tested.

If all three tests pass, this is the best path to the exact user-message experience. If text works but audio does not, it may still be a better direct-chat route after transcription. A Shortcut that requires an unlock, confirmation, or foreground switch does not meet the goal.

## Best supported automatic route if Shortcuts fails

```
button held → BLE audio → native iPhone receiver → transcript → authenticated Bot webhook routine
button released → one webhook POST → Bot acts → result in Grok Bot app
```

A native iPhone app receives the custom BLE notifications, reconstructs each take, transcribes the WAV with Apple's Speech framework, and sends the text in the JSON body of a Grok Bot webhook routine. This avoids email, a mail relay, and a separate speech-to-text account. Keep the WAV locally for a short retry window, and mark a take delivered only after the webhook accepts it. The Bot's routine instruction should say how to treat a pendant transcript and where to report results; normal approval rules still apply.

Apple provides file-based speech recognition and newer SpeechAnalyzer APIs, but actual language support, permission flow, offline behavior, transcription speed, and locked-phone execution need tests on Max's iPhone. If on-device transcription cannot finish reliably in the background, the app can upload the WAV to a small authenticated server that transcribes it and calls the Bot webhook. That server is a fallback, not a starting requirement.

**Tradeoff:** this sends the Bot the *words*, not an ordinary voice-message attachment. For commands and notes, that may meet the real goal. If Grok Bot needs the original tone or acoustic context, keep the audio path below.

## Audio-first fallback that Max has partly validated

The iPhone receiver can upload the completed WAV to a mail service, which emails it to the Bot's own address and triggers an email routine. Max has already verified that Grok Bot understands emailed audio. The remaining tests are unattended sending, routine behavior, latency, and whether the result appears in the desired conversation. Apple's Mail compose interface requires a Send tap, so the receiver must use an email API or a small relay.

A webhook JSON body is documented for text; direct audio attachment ingestion is not. A short-lived, access-controlled audio URL in the JSON body is another experiment, but it adds file storage and Bot-side download. Do not claim it is equivalent to a normal voice message without testing.

## iPhone and firmware work

The receiver must use Core Bluetooth, subscribe to AUDIO and STATE, decode the 165-byte ADPCM frames as `pendant_rx.py` does, and end a take when recording stops. It should queue retries, avoid duplicate delivery, and report failures. Use `bluetooth-central` background mode and state restoration. Apple allows background BLE events but limits execution and may not relaunch an app that the user force-quits. **A locked-phone recording is the first engineering gate.**

The current firmware streams 20 ms frames live and drops them when the phone is disconnected. Since the desired interaction is a message sent *after release*, consider recording compressed frames to the XIAO Sense's 2 MB onboard QSPI flash and transferring the take after release with acknowledgement. At 8 kB/s of IMA-ADPCM payload, 2 MB is roughly four minutes before filesystem overhead. This is a design estimate, not implemented or tested. It could make brief BLE interruptions recoverable and reduce the need for continuous background packet handling, but adds firmware and flash-management work. Test the existing stream first; change to store-and-forward if locked-phone reliability requires it.

## Why a normal Bluetooth microphone is another project

A standard Bluetooth headset mic could be used by Grok Bot's built-in voice chat, potentially avoiding a custom BLE receiver. The present nRF52840 board is a custom BLE data device; it does not support Bluetooth Classic HFP or the supported LE Audio stack needed to appear as a system microphone. Switching to headset hardware and firmware would change the MVP, and Grok Bot voice chat is a live call rather than one hold/release message. Test that route with existing earbuds if the product goal changes.

## Test order

1. **Today, no hardware:** test Grok Bot's Send Message Shortcut with fixed text, then a WAV; test confirmation and Lock Screen behavior. Also time the already-working email-audio route.
2. **Bot ingress:** if the Shortcut fails, create a webhook routine and send one test transcript to it. Verify that the result appears in the expected Bot conversation and preserves useful context.
3. **Pendant bench:** confirm pairing and one complete WAV using the existing laptop receiver.
4. **iPhone receiver:** receive and decode a take with the phone unlocked, then locked and in a pocket. Test reconnect, long takes, and force-quit behavior.
5. **End to end:** release the pendant button and verify exactly one request reaches Grok Bot with no phone interaction. Measure latency and failure rate. Only then decide whether email, audio URLs, or store-and-forward are necessary.

## Primary references

- [Grok Bot iPhone Shortcuts and share-sheet behavior](https://docs.x.ai/grok-bot/mobile)
- [Grok Bot webhook and email routines](https://docs.x.ai/grok-bot/skills-routines-and-automations)
- [Apple: run a Shortcut using a URL](https://support.apple.com/guide/shortcuts/run-a-shortcut-from-a-url-apd624386f42/ios)
- [Apple: speech recognition from an audio file](https://developer.apple.com/documentation/speech/sfspeechurlrecognitionrequest)
- [Apple: Core Bluetooth background processing](https://developer.apple.com/library/archive/documentation/NetworkingInternetWeb/Conceptual/CoreBluetooth_concepts/CoreBluetoothBackgroundProcessingForIOSApps/PerformingTasksWhileYourAppIsInTheBackground.html)
- [Apple: background Bluetooth relaunch rules](https://developer.apple.com/documentation/technotes/tn3115-bluetooth-state-restoration-app-relaunch-rules)
- [Apple: Mail compose requires the user to send](https://developer.apple.com/documentation/messageui/mfmailcomposeviewcontroller)
- [Seeed: 2 MB onboard flash on XIAO nRF52840 Sense](https://wiki.seeedstudio.com/XIAO_BLE/)
- [Nordic: nRF52840 LE Audio support limitation](https://devzone.nordicsemi.com/f/nordic-q-a/77227/does-or-will-the-nrf52840-support-le-audio/319243)
