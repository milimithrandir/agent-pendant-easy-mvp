# Step 0: free test, no hardware (€0, today)

**Question it answers:** is "hold a button, say something, get an answer on the phone" actually useful in your day, before you buy anything? The pendant later only replaces the phone's mic with a BLE mic.

## Easiest way (5 minutes)
1. Put in your Bluetooth earbuds (their mic is roughly where a pendant mic would be: close to your mouth, not in your hand).
2. Use the **hold-to-record voice message** button in the chat app you'd use with the agent (Telegram, WhatsApp, X Chat, or the agent's own app). That *is* hold-to-talk.
3. For 2–3 days, every time you'd want the pendant, send the agent a voice note instead. Notice:
   - how long you wait for the answer (latency);
   - what you actually say to it (commands? notes? questions?);
   - whether you'd rather press on your chest than take the phone out.
4. Write the answers in a note. They decide the phone app and the agent integration more than any hardware does.

## Optional: the hold-to-talk page
[`step0/hold_to_talk.html`](step0/hold_to_talk.html) is a one-file page with one big button: hold to record, release to stop, then play / save the clip.
- Laptop: `cd step0 && python3 -m http.server 8000`, open http://localhost:8000/hold_to_talk.html.
- Phone: browsers only allow the mic on **https**, so host the file on any static https host, or just use the chat-app method above.
- It's the seed of the phone receiver: on Android, Chrome's Web Bluetooth can later receive the pendant's audio and CO2 on the same page. iPhone Safari has no Web Bluetooth (needs a small native app, or the Bluefy browser for tests; Bluefy with the passkey-protected characteristics is unverified).

## What Step 0 can't tell you
BLE audio quality and range, battery life, CO2 on the body, how it feels to wear and press. That's what the €49–56 build is for.
