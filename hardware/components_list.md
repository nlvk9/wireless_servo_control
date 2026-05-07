# Components List

## Bill of Materials

| Component | Part / Example | Qty (per pair) | Notes |
|-----------|---------------|---------------|-------|
| ESP32 Dev Board | AZ-Delivery 38-pin | 2 | Any 38-pin layout works |
| Hobby servo — standard | MG996R (metal gear) | 1–2 | 5 V; 500–2500 µs pulse range |
| Momentary push button | 6 mm tactile switch | 1–2 | No external resistor needed — uses internal pull-up |
| 5 mm LED | Any colour | 0–2 | Pair B only |
| Current-limiting resistor | 330 Ω | 0–2 | For LEDs |
| 5 V regulated supply | LM7805 or switching module | 1 per board | Servos can draw 1–2 A stall — do not rely on USB power |
| Decoupling capacitor | 100 µF electrolytic | 1 per servo | Across servo power rails to reduce voltage spikes |
| Jumper wires / PCB | — | — | Breadboard for prototyping |

---

## GPIO Quick Reference

### Pair A — 1 Button / 1 Door

| Role | GPIO | Description |
|------|------|-------------|
| **Sender** | | |
| Button 1 | 26 | INPUT_PULLUP |
| Button 2 | 25 | INPUT_PULLUP |
| **Receiver** | | |
| Servo 1 (door) | 32 | PWM output |
| Servo 2 (secondary) | 22 | PWM output |

### Pair B — 2 Buttons / 1 Door + 1 Meteor

| Role | GPIO | Description |
|------|------|-------------|
| **Sender** | | |
| Button 1 (door) | 27 | INPUT_PULLUP |
| Button 2 (meteor) | 25 | INPUT_PULLUP |
| **Receiver** | | |
| Servo 1 (door) | 33 | PWM output |
| Servo 2 (meteor) | 22 | PWM output |
| White LED | 27 | Door status |
| Blue LED | 26 | Meteor stopped indicator |

### Pair C — 1 Button / 2 Doors

| Role | GPIO | Description |
|------|------|-------------|
| **Sender** | | |
| Button 1 | 26 | INPUT_PULLUP |
| **Receiver** | | |
| Servo 1 (door 1) | 32 | PWM output |
| Servo 2 (door 2) | 33 | PWM output |

---

## Power Notes

- Servos under load can spike to 1–2 A. Power them from a dedicated 5 V supply, **not** the ESP32's 3V3 rail.
- Place a 100 µF electrolytic capacitor across the servo power rails to absorb transients.
- The ESP32 itself can be powered via USB during development; use a dedicated 3.3 V regulator in production builds.
