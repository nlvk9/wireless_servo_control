# Changelog

All notable changes to this project will be documented here.

Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

---

## [Unreleased]

## [1.0.0] — 2025-XX-XX

### Added
- Initial release with three sender/receiver firmware pairs:
  - `sender_1btn_1door` / `receiver_1btn_1door`
  - `sender_2btn_1door_1meteor` / `receiver_2btn_1door_1meteor`
  - `sender_1btn_2doors` / `receiver_1btn_2doors`
- Shared `packet_types.h` — canonical ESP-NOW struct definitions
- Shared `constants.h` — servo defaults, debounce timing, button logic levels
- FreeRTOS sweep task (core 0) in `receiver_2btn_1door_1meteor`
- Deferred servo detach to eliminate post-move jitter in door pair B
- PlatformIO configuration for all six firmware targets
- `tools/get_mac_address` utility sketch
