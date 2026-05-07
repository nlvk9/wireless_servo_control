# Contributing

Thank you for your interest in contributing! This document covers the
conventions used in the codebase so that pull requests stay consistent.

---

## Getting Started

1. Fork the repository and clone your fork.
2. Install [PlatformIO](https://platformio.org/) (VS Code extension or CLI).
3. Confirm you can build a firmware target before making changes:
   ```bash
   cd firmware/sender_1btn_1door
   pio run
   ```

---

## Project Conventions

### Naming

| Thing | Convention | Example |
|-------|-----------|---------|
| Firmware folder | `{role}_{Nbtn}_{actuators}` | `sender_2btn_1door_1meteor` |
| Source file | Same as folder | `sender_2btn_1door_1meteor.ino` |
| Global variables | `camelCase` with `static` | `static int lastBtn1` |
| Constants / config | `UPPER_SNAKE_CASE` | `static const int SERVO1_PIN` |
| Callbacks | `onDataSent`, `onDataReceived` | — |
| FreeRTOS tasks | `task` + descriptive name | `taskMeteorSweep` |

### Code Structure

Every firmware file must follow this section order:

```
File docblock (pair info, packet type, wiring)
─────────────────────────────────────────────
#include directives
── Configuration (pins, angles, timing)
── State (packet, servos, last-button tracking)
── FreeRTOS tasks (receivers with sweep only)
── Helpers (moveDoor, etc.)
── Callbacks (onDataSent / onDataReceived)
── setup()
── loop()   ← always empty or near-empty
```

### Shared Headers

- **Never duplicate** struct definitions in individual firmware files — always include from `shared/packet_types.h`.
- Constants that appear in more than one file belong in `shared/constants.h`.

### ESP-NOW Structs

- The sender and receiver in a pair **must use the identical struct**.
- If you add a field, update **both** sides and the packet type comments.

---

## Adding a New Firmware Pair

1. Create `firmware/sender_<name>/src/sender_<name>.ino` and `firmware/receiver_<name>/src/receiver_<name>.ino`.
2. Copy the nearest existing `platformio.ini` into both new folders and update the env name.
3. Use an existing packet struct from `shared/packet_types.h`, or add a new one there (not inline).
4. Add the pair to the table in `README.md`.
5. Document the wiring in `hardware/components_list.md`.

---

## Pull Request Checklist

- [ ] Code builds with `pio run` (no errors, no warnings)
- [ ] Sender and receiver struct definitions match
- [ ] New constants are in `shared/constants.h`, not hard-coded
- [ ] Serial log messages follow the `[INFO]` / `[ERROR]` prefix style
- [ ] `README.md` pair table updated (if adding a new pair)
- [ ] `CHANGELOG.md` updated under `[Unreleased]`

---

## Reporting Issues

Please include:
- Which firmware pair (sender + receiver names)
- ESP32 board variant and PlatformIO version
- Serial Monitor output from both boards
- Steps to reproduce
