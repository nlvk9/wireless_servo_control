/**
 * @file receiver_1btn_1door.ino
 * @brief ESP-NOW receiver: button 1 toggles servo 1 (door);
 *        button 2 toggles servo 2 (secondary door / accessory).
 *
 * Packet type : TwoButtonPacket
 * Matched with: sender_1btn_1door
 *
 * Wiring
 * ──────
 * Servo 1 (door)      → GPIO 32
 * Servo 2 (secondary) → GPIO 22
 */

#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>

#include "packet_types.h"
#include "constants.h"


/* ── Configuration ───────────────────────────────────────────────────────── */

static const int SERVO1_PIN = 32;
static const int SERVO2_PIN = 22;

/** Door travel range in degrees. Adjust to match physical hardware. */
static const int SERVO1_OPEN_DEG  = 75;
static const int SERVO1_CLOSE_DEG = 0;

static const int SERVO2_OPEN_DEG  = 90;
static const int SERVO2_CLOSE_DEG = 0;


/* ── State ───────────────────────────────────────────────────────────────── */

static TwoButtonPacket inPacket;

static Servo servo1;
static Servo servo2;

static int servo1Angle = SERVO1_CLOSE_DEG;
static int servo2Angle = SERVO2_CLOSE_DEG;

static int lastBtn1 = BTN_NOT_PRESSED;
static int lastBtn2 = BTN_NOT_PRESSED;


/* ── Callbacks ───────────────────────────────────────────────────────────── */

/** Called automatically whenever an ESP-NOW packet arrives. */
void onDataReceived(const esp_now_recv_info* info, const uint8_t* data, int len) {

  memcpy(&inPacket, data, sizeof(inPacket));

  /* Button 1 rising-edge: toggle servo 1 (door). */
  if (inPacket.button1State == BTN_PRESSED && lastBtn1 == BTN_NOT_PRESSED) {
    servo1Angle = (servo1Angle == SERVO1_CLOSE_DEG) ? SERVO1_OPEN_DEG : SERVO1_CLOSE_DEG;
    servo1.write(servo1Angle);
    Serial.printf("[INFO] Button 1 → servo 1 → %d°\n", servo1Angle);
  }

  /* Button 2 rising-edge: toggle servo 2 (secondary). */
  if (inPacket.button2State == BTN_PRESSED && lastBtn2 == BTN_NOT_PRESSED) {
    servo2Angle = (servo2Angle == SERVO2_CLOSE_DEG) ? SERVO2_OPEN_DEG : SERVO2_CLOSE_DEG;
    servo2.write(servo2Angle);
    Serial.printf("[INFO] Button 2 → servo 2 → %d°\n", servo2Angle);
  }

  lastBtn1 = inPacket.button1State;
  lastBtn2 = inPacket.button2State;
}


/* ── Setup ───────────────────────────────────────────────────────────────── */

void setup() {
  Serial.begin(115200);

  servo1.setPeriodHertz(SERVO_FREQ_HZ);
  servo2.setPeriodHertz(SERVO_FREQ_HZ);
  servo1.attach(SERVO1_PIN, SERVO_PULSE_MIN, SERVO_PULSE_MAX);
  servo2.attach(SERVO2_PIN, SERVO_PULSE_MIN, SERVO_PULSE_MAX);

  /* Move servos to their known starting positions. */
  servo1.write(SERVO1_CLOSE_DEG);
  servo2.write(SERVO2_CLOSE_DEG);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("[ERROR] ESP-NOW init failed — halting.");
    while (true) {}
  }

  esp_now_register_recv_cb((esp_now_recv_cb_t)onDataReceived);

  Serial.println("[INFO] Receiver ready — waiting for packets.");
}


/* ── Main loop ───────────────────────────────────────────────────────────── */

void loop() {
  /* All logic is handled in the ESP-NOW callback. */
}
