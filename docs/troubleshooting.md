# Troubleshooting

## Sender prints "Send FAILED"

| Cause | Fix |
|-------|-----|
| Wrong MAC address in sender | Re-flash `tools/get_mac_address` to the receiver and copy the printed address |
| Receiver not powered | Power the receiver board first; ESP-NOW needs the peer to be registered |
| Both boards on different Wi-Fi channels | Set `peerInfo.channel = 0` on the sender (uses current channel automatically) |

## Receiver gets no data

- Open Serial Monitor on the receiver at **115200 baud** and confirm `[INFO] Receiver ready` appears on boot.
- Confirm the sender's `RECEIVER_MAC` matches the receiver's actual MAC (use `get_mac_address` sketch).
- Confirm both boards use the **same struct** (`TwoButtonPacket` or `SingleButtonPacket`). Mismatched structs cause silent data corruption with no error message.

## Servo jitters after moving

- The door servo in Pair B detaches itself 600 ms after a move command. If jitter appears before that window, increase the delay in `moveDoor()`.
- Ensure the servo is powered from a **dedicated 5 V supply**, not the ESP32's 3V3 rail.
- Add a 100 µF capacitor across the servo power rails.

## Meteor sweep is jerky

- Reduce `SWEEP_STEP_DEG` (smaller steps) or increase `SWEEP_TICK_MS` (slower ticks).
- Confirm the sweep task is pinned to **core 0** (`xTaskCreatePinnedToCore(..., 0)`). If it runs on core 1 alongside the ESP-NOW callback, they compete.

## Button triggers multiple times per press

- Increase `DEBOUNCE_MS` in `shared/constants.h` from 50 ms to 80–100 ms.
- Check for a loose button connection — floating inputs cause phantom triggers.

## Build fails: "packet_types.h not found"

- Confirm `build_flags = -I../../shared` is present in the target's `platformio.ini`.
- Check that `shared/` is at the repository root (two levels above each firmware `src/` folder).

## ESP-NOW init failed on boot

- Ensure `WiFi.mode(WIFI_STA)` is called **before** `esp_now_init()`.
- Try a full power cycle (not just reset) — the Wi-Fi radio occasionally fails to re-initialise after a software reset.
