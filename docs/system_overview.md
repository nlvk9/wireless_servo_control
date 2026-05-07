# System Overview

## Architecture

Each animatronic prop is controlled by a matched sender/receiver pair of ESP32 boards communicating over ESP-NOW.

```
                      2.4 GHz (ESP-NOW)
  ┌─────────────┐ ──────────────────────► ┌──────────────────┐
  │ Sender ESP32│                          │ Receiver ESP32   │
  │             │                          │                  │
  │ • Button(s) │   TwoButtonPacket  or    │ • Servo(s)       │
  │ • Debounce  │   SingleButtonPacket     │ • LED(s)         │
  │ • Edge det. │                          │ • FreeRTOS tasks │
  └─────────────┘                          └──────────────────┘
```

## Sender Responsibilities

1. Read button GPIO states on every loop iteration.
2. Detect a state change on any button.
3. Apply a 50 ms debounce and re-confirm the change.
4. Pack the confirmed states into the appropriate packet struct.
5. Transmit the packet via `esp_now_send()`.

## Receiver Responsibilities

1. Register an ESP-NOW receive callback (`onDataReceived`).
2. On each packet: `memcpy` into the local struct, then detect rising edges per button field.
3. Drive servo(s) and LED(s) based on which button edge was detected.
4. For Pair B: run a FreeRTOS sweep task on core 0 independently of the callback on core 1.

## Core Assignment (Pair B only)

| Core | Task |
|------|------|
| Core 0 | `taskMeteorSweep` — continuous servo sweep |
| Core 1 | ESP-NOW stack + `onDataReceived` callback (default Arduino core) |

Pinning the sweep to core 0 prevents the servo PWM timing from being disrupted by incoming packet processing.

## Packet Flow

```
Button press detected
       │
       ▼
Debounce (50 ms)
       │
       ▼
Re-read GPIOs — change confirmed?
       │ yes
       ▼
Fill packet struct
       │
       ▼
esp_now_send() ──► [radio] ──► OnDataRecv() on receiver
                                      │
                                      ▼
                               Detect rising edge
                                      │
                                      ▼
                               Drive actuator(s)
```
