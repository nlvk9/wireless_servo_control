#pragma once

/**
 * @file constants.h
 * @brief Project-wide constants shared across all firmware targets.
 */

/* ── Button logic ────────────────────────────────────────────────────────── */
static const int BTN_PRESSED     = LOW;
static const int BTN_NOT_PRESSED = HIGH;

/* ── Debounce ────────────────────────────────────────────────────────────── */
static const int DEBOUNCE_MS = 50;   // Milliseconds to wait after a state change

/* ── Servo defaults ──────────────────────────────────────────────────────── */
static const int SERVO_FREQ_HZ    = 50;    // Standard hobby servo PWM frequency
static const int SERVO_PULSE_MIN  = 500;   // Minimum pulse width in microseconds
static const int SERVO_PULSE_MAX  = 2500;  // Maximum pulse width in microseconds

/* ── ESP-NOW ─────────────────────────────────────────────────────────────── */
static const int ESPNOW_CHANNEL   = 0;     // 0 = use current WiFi channel
