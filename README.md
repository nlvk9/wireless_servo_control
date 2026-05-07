# ESP-NOW Wireless Servo Control

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![Framework: Arduino](https://img.shields.io/badge/Framework-Arduino-teal.svg)](https://www.arduino.cc/)

Wireless servo controller system built on the **ESP32** using **ESP-NOW**, a wireless, low-latency Wi-Fi protocol that requires no router, no pairing ceremony, and reaches full operation in milliseconds.

Each sender/receiver pair operates independently. When a button is pressed on the sender, the matched receiver reacts instantly.

---

## Table of Contents

- [System Overview](#system-overview)
- [Firmware Pairs](#firmware-pairs)
- [Repository Structure](#repository-structure)
- [Quick Start](#quick-start)
- [Hardware](#hardware)
- [Shared Code](#shared-code)
- [Tools](#tools)
- [Contributing](#contributing)
- [License](#license)

---

## System Overview

```
Sender ESP32 -- ESP-NOW --> Receiver ESP32
```

- ESP-NOW is peer-to-peer. This means no router is necessary.
- Under 10-ms latency, meaning it is highly suitable for live performance.
- Each sender is bound to a receiver based on the receiver's MAC address.
- FreeRTOS is used to handle multiple tasks simultaneously at once on an ESP32, using its dual-core processing capabilities.

---

## Pairs

| Pair | Sender | Receiver | Buttons | Actuators |
|------|--------|----------|---------|-----------|
| A | `sender_1btn_1door` | `receiver_1btn_1door` | 2 buttons | 2 servos (independent servo toggle) |
| B | `sender_2btn_1door_1meteor` | `receiver_2btn_1door_1meteor` | 2 buttons | Door servo + continuous sweep servo + 2 LEDs (optional LEDs) |
| C | `sender_1btn_2doors` | `receiver_1btn_2doors` | 1 button | 2 servos (synchronized door toggle) |

> Pair B runs the meteor sweep on a dedicated FreeRTOS task (core 0) so the ESP-NOW callback (core 1) is never blocked.

---

## Repository Structure

```
esp-now-animatronics/
│
├── firmware/
│   ├── sender_1btn_1door/
│   │   ├── src/sender_1btn_1door.ino
│   │   └── platformio.ini
│   ├── sender_2btn_1door_1meteor/
│   │   ├── src/sender_2btn_1door_1meteor.ino
│   │   └── platformio.ini
│   ├── sender_1btn_2doors/
│   │   ├── src/sender_1btn_2doors.ino
│   │   └── platformio.ini
│   ├── receiver_1btn_1door/
│   │   ├── src/receiver_1btn_1door.ino
│   │   └── platformio.ini
│   ├── receiver_2btn_1door_1meteor/
│   │   ├── src/receiver_2btn_1door_1meteor.ino
│   │   └── platformio.ini
│   └── receiver_1btn_2doors/
│       ├── src/receiver_1btn_2doors.ino
│       └── platformio.ini
│
├── shared/
│   ├── packet_types.h   ← ESP-NOW struct definitions (must match sender & receiver)
│   └── constants.h      ← Pin constants, servo defaults, timing values
│
├── hardware/
│   ├── wiring_diagrams/ ← Fritzing / KiCad schematics
│   ├── pcb_files/       ← Gerber files (if applicable)
│   └── components_list.md
│
├── tools/
│   ├── get_mac_address/ ← Utility sketch: prints ESP32 MAC to Serial Monitor
│   ├── packet_decoder.py
│   └── test_sender.py
│
├── docs/
│   ├── system_overview.md
│   ├── communication_protocol.md
│   └── troubleshooting.md
│
├── .github/
│   └── workflows/
│       └── build-check.yml
│
├── CHANGELOG.md
├── CONTRIBUTING.md
├── LICENSE
├── .gitignore
└── README.md
```

---

## Quick Start

### Prerequisites

- The repository is built to work on VSCode (with the Platform IO extension).
- Two ESP32 development boards per pair.
- Buttons (1 per transmitter).
- Servos (2 per receiver).
- External batteries, depending on the servo voltage intake. When I made this project, I used two 3.2V lithium iron phosphate batteries connected in series (using spot welding to connect them on one side, spot welding individual tabs on the other side for each battery, then soldering wires onto the individual tabs) to power the servos I used.

### 1 — Find receiver MAC addresses
Flash the code on this website: https://randomnerdtutorials.com/get-change-esp32-esp8266-mac-address-arduino/ to get the MAC address. You can find it in the Serial Monitor.

### 2 — Configure the sender

Open the matching sender `.ino` and update `RECEIVER_MAC`:

```cpp
static uint8_t RECEIVER_MAC[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
```

### 3 — Flash sender and receiver

Using PlatformIO (from the project root):

```bash
cd firmware/sender_1btn_1door
pio run --target upload

cd ../receiver_1btn_1door
pio run --target upload
```

### 4 — Test

Open the Serial Monitor at **115200 baud** on both boards. Press the button on the sender — you should see:

```
[ESP-NOW] Send OK          ← on sender
[INFO] Button 1 → servo 1 → 75°   ← on receiver
```

---

## Hardware

See [`hardware/components_list.md`](hardware/components_list.md) for the full bill of materials and wiring reference.

| Component | Qty | Notes |
|-----------|-----|-------|
| ESP32 Dev Board (38-pin) | 2× per pair | Any standard 38-pin layout |
| Hobby servo (e.g. MG996R) | 1–2 per receiver | 5 V, 500–2500 µs pulse |
| Momentary push button | 1–2 per sender | Uses internal pull-up; no external resistor needed |
| 5 mm LED + 330 Ω resistor | 0–2 per receiver | Pair B only |
| 5 V regulated power supply | 1 per board | Servos draw significant current — do not power from USB alone |

---

## Shared Code

All sender/receiver pairs import two headers from `shared/`:

| File | Purpose |
|------|---------|
| `packet_types.h` | Defines `TwoButtonPacket` and `SingleButtonPacket`. **Both boards in a pair must use the same struct or data will be silently corrupted.** |
| `constants.h` | Servo frequency, pulse widths, debounce timing, button logic levels |

---

## Tools

| Tool | Purpose |
|------|---------|
| `tools/get_mac_address/` | Arduino sketch — prints the board's MAC address to Serial Monitor |
| `tools/packet_decoder.py` | Decode raw ESP-NOW packet bytes for debugging |
| `tools/test_sender.py` | Simulate sender packets over serial for bench testing |

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

---

## License

[MIT](LICENSE) — © 2026 Neel Vinayak
