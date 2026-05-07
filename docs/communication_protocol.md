# Communication Protocol

## Transport

| Property | Value |
|----------|-------|
| Protocol | ESP-NOW (Espressif proprietary, 802.11 layer) |
| Frequency | 2.4 GHz Wi-Fi |
| Topology | Unicast peer-to-peer (one sender → one receiver) |
| Encryption | Disabled (add PMK/LMK for sensitive deployments) |
| Max payload | 250 bytes (ESP-NOW limit) |
| Typical latency | < 10 ms |

---

## Packet Structures

Defined in [`shared/packet_types.h`](../shared/packet_types.h).

### `TwoButtonPacket`

Used by pairs A and B.

```c
typedef struct {
  int id;            // Board identifier
  int button1State;  // LOW = pressed, HIGH = released
  int button2State;  // LOW = pressed, HIGH = released
} TwoButtonPacket;
```

Total size: 12 bytes (3 × `int` on ESP32).

### `SingleButtonPacket`

Used by pair C.

```c
typedef struct {
  int id;            // Board identifier
  int button1State;  // LOW = pressed, HIGH = released
  int pressCount;    // Cumulative press counter
} SingleButtonPacket;
```

Total size: 12 bytes.

---

## Transmission Rules

- Senders transmit **only on state change** — no polling packets.
- A 50 ms software debounce is applied before transmission.
- Receivers act on the **rising edge** of a button press (`LOW` after `HIGH`) to avoid double-triggers.
- The `pressCount` field in `SingleButtonPacket` lets the receiver verify it has not missed a packet.

---

## Adding a New Packet Type

1. Define the new struct in `shared/packet_types.h` with a comment listing which pairs use it.
2. Both sender and receiver **must** use `sizeof(YourStruct)` in their `esp_now_send` / `memcpy` calls.
3. Never cast to a different struct type on either end — this causes silent data corruption.
