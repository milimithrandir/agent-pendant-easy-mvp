# Step 0: find the simplest automatic path to Grok Bot

No pendant hardware is needed for these tests. The product goal is: hold the pendant button, speak, release, and have Grok Bot act without touching the iPhone. A manual test here checks an integration capability; it is not the final interaction.

## 1. Test Grok Bot's direct iPhone Shortcut

In the iPhone Shortcuts app, create a Shortcut with **Grok Bot → Send Message** aimed at your intended Bot.

1. Start with fixed text such as “Pendant test: reply with the word received.” Run it. Did the message appear directly in your normal Bot chat, with no extra Send tap?
2. Feed a short WAV or M4A voice memo into the action as Shortcut input. Does it accept the audio, send it, and let the Bot act? Record whether it opens a composer.
3. Try running the Shortcut from the Lock Screen. If it needs an unlock or confirmation, record that. Apple documents a URL scheme through which another app can run a Shortcut with **text** input, but automatic locked-phone execution from a background BLE event is still unverified.

This is the only documented in-app lead for a direct Bot message. Do not build the iPhone receiver around it until these tests pass.

## 2. Test the automatic webhook route if Shortcuts is insufficient

Ask the intended Grok Bot to make a **webhook-triggered routine** that treats the JSON `text` field as a pendant transcript, acts on it under the usual approval rules, and reports its result in the Bot conversation. Open the routine on desktop to copy its URL and key. From a trusted laptop, send a simple test JSON message to that URL as documented in [Grok Bot routines](https://docs.x.ai/grok-bot/skills-routines-and-automations). A successful response means the run **started**, not that the Bot completed the request. Check the chat and run history.

A future iPhone receiver could transcribe each recording with Apple's Speech framework and POST the transcript itself. That would avoid an email service, but it would send text rather than an ordinary voice attachment.

## 3. Keep the already-working audio path as fallback

Max has confirmed that his Bot understands audio sent by email. Time one more email-triggered run and note where the result appears. If a direct Shortcut and transcript webhook do not meet the need, this remains a way for an iPhone receiver to deliver the **original WAV** automatically. The receiver would need an email API or relay: Apple's Mail compose screen requires a Send tap.

## Still unproven

- iPhone reception of the pendant's custom BLE audio while locked, including reconnect and long takes.
- Whether the Send Message Shortcut can send a file and run unattended.
- Whether a webhook routine has the same conversation feel and context as a normal user message.
- Whether recording to the XIAO Sense's onboard flash before BLE transfer is needed for reliable delivery.

See [`VOICE_TO_GROK_BOT.md`](VOICE_TO_GROK_BOT.md) for the architecture and evidence. The optional [`step0/hold_to_talk.html`](step0/hold_to_talk.html) records local clips; it does not send them to Grok Bot.
