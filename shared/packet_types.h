#pragma once

/**
 * @file packet_types.h
 * @brief Shared ESP-NOW packet structures used across all sender/receiver pairs.
 *
 * IMPORTANT: Both the sender and receiver in a matched pair must use the
 * identical struct. Any mismatch will cause silent data corruption.
 */

/* ── Two-button packet (button_state + button2_state) ───────────────────────
 * Used by:
 *   sender_1btn_1door   ↔  receiver_1btn_1door
 *   sender_2btn_1door_1meteor  ↔  receiver_2btn_1door_1meteor
 */
typedef struct {
  int id;            // Board identifier
  int button1State;  // LOW = pressed, HIGH = released
  int button2State;  // LOW = pressed, HIGH = released
} TwoButtonPacket;


/* ── Single-button + counter packet ─────────────────────────────────────────
 * Used by:
 *   sender_1btn_2doors  ↔  receiver_1btn_2doors
 */
typedef struct {
  int id;              // Board identifier
  int button1State;    // LOW = pressed, HIGH = released
  int pressCount;      // Cumulative button press counter
} SingleButtonPacket;
