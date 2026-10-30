# Step 0: test Grok Bot delivery before buying hardware

**Question:** can a short spoken request reach Max's existing Grok Bot and cause the intended response with no phone action after the recording ends? This test costs no pendant hardware. It does **not** prove that an iPhone can receive pendant audio while locked.

## A. Check that Grok Bot understands the recording

1. Record a 5–10 second request as a WAV or M4A on the iPhone.
2. Attach it manually in the Grok Bot app and send it to the Bot you want the pendant to address.
3. Note whether it transcribes the request correctly, acts on it, and replies in the conversation you expect.

The manual attachment is a **capability test**, not the final interaction. The final goal remains: hold the pendant button, speak, release, and do nothing else.

## B. Check automatic audio delivery

1. Set up the Bot's own email address if you want to test the audio-email route. In that Bot, create an **email-triggered routine** limited to messages from your test sender. Its instruction should identify the audio attachment as a pendant recording, ask the Bot to transcribe and handle the spoken request, and report the result in the Bot conversation. Keep the Bot's existing approval rules.
2. Send the same recording to that address using an email client or a test mail service. Note whether the routine starts, can read the attachment, understands the speech, and reports back without another prompt.
3. Record the time from send to result and any approval request. Repeat with a second recording to check whether the behavior is consistent.

This tests the Bot-side route. A future iPhone receiver still needs to send the mail **programmatically**; opening the Mail compose sheet would require a tap. Email content is not treated as permission for the Bot to take actions, so test the exact kinds of requests you expect.

## C. Fallback if email does not meet the goal

Test a Grok Bot **webhook-triggered routine** with a JSON body containing a transcript. It is a route for automatic text delivery, but obtaining the transcript requires speech-to-text on the phone or a separate service. The webhook documentation does not establish that it accepts an audio attachment as a normal voice message. See [`VOICE_TO_GROK_BOT.md`](VOICE_TO_GROK_BOT.md) before choosing this route.

## What remains unproven

- The XIAO firmware, battery, microphone, BLE pairing, and audio quality on real hardware.
- Core Bluetooth audio reception while the iPhone is locked, reconnecting, or briefly offline.
- Whether a routine result appears and behaves exactly like a normal user voice message in the existing Bot chat.

The optional [`step0/hold_to_talk.html`](step0/hold_to_talk.html) records and saves local clips. It does not send them to Grok Bot. On an iPhone it must be hosted on HTTPS to use the microphone.

## References

- [Grok Bot audio attachments](https://docs.x.ai/grok-bot/files-and-results)
- [Grok Bot email and webhook routines](https://docs.x.ai/grok-bot/skills-routines-and-automations)
- [Grok Bot own email and email trust model](https://docs.x.ai/grok-bot/computer-and-apps)
