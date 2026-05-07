# ESP-NOW Animatronics Controller

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![Framework: Arduino](https://img.shields.io/badge/Framework-Arduino-teal.svg)](https://www.arduino.cc/)

Wireless animatronic prop controller system built on the **ESP32** using **ESP-NOW** — a connectionless, low-latency Wi-Fi protocol that requires no router, no pairing ceremony, and reaches full operation in milliseconds.

Each sender/receiver pair operates independently. Press a button on the sender; the matched receiver reacts instantly across the room.

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
[Sender ESP32] ──── ESP-NOW (2.4 GHz) ────► [Receiver ESP32]
   (button input)                              (servo / LED output)
```

- **No router required** — ESP-NOW is peer-to-peer
- **Sub-10 ms latency** — suitable for live performance
- **Independent pairs** — each sender is bound to one receiver by MAC address
- **FreeRTOS** — multi-core tasks keep sweeping servos smooth and non-blocking

---

## Firmware Pairs

| Pair | Sender | Receiver | Buttons | Actuators |
|------|--------|----------|---------|-----------|
| A | `sender_1btn_1door` | `receiver_1btn_1door` | 2 buttons | 2 servos (independent door toggle) |
| B | `sender_2btn_1door_1meteor` | `receiver_2btn_1door_1meteor` | 2 buttons | Door servo + continuous sweep servo + 2 LEDs |
| C | `sender_1btn_2doors` | `receiver_1btn_2doors` | 1 button | 2 servos (synchronized door toggle) |

> **Pair B** runs the meteor sweep on a dedicated FreeRTOS task (core 0) so the ESP-NOW callback (core 1) is never blocked.

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

- [PlatformIO](https://platformio.org/) (VS Code extension recommended)  
  *or* Arduino IDE 2.x with the ESP32 board package installed
- Two ESP32 development boards per prop

### 1 — Find receiver MAC addresses

Flash the `tools/get_mac_address` sketch to each **receiver** board and note the printed MAC address in the Serial Monitor.

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

[MIT](LICENSE) — © 2025 Your Name
