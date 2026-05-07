/**
 * @file receiver_1btn_2doors.ino
 * @brief ESP-NOW receiver: each button press toggles two door servos in
 *        unison between 0° and 90°.
 *
 * Packet type : SingleButtonPacket
 * Matched with: sender_1btn_2doors
 *
 * Wiring
 * ──────
 * Servo 1 (door 1) → GPIO 32
 * Servo 2 (door 2) → GPIO 33
 */

#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>

#include "packet_types.h"
#include "constants.h"


/* ── Configuration ───────────────────────────────────────────────────────── */

static const int SERVO1_PIN = 32;
static const int SERVO2_PIN = 33;

/** Door travel range in degrees. Adjust to match physical hardware. */
static const int DOOR_OPEN_DEG  = 90;
static const int DOOR_CLOSE_DEG = 0;


/* ── State ───────────────────────────────────────────────────────────────── */

static SingleButtonPacket inPacket;

static Servo servo1;
static Servo servo2;

/**
 * Tracks whether the doors are currently open or closed.
 * Toggled on every confirmed button press from the sender.
 */
static bool doorsOpen = false;


/* ── Callbacks ───────────────────────────────────────────────────────────── */

/** Called automatically whenever an ESP-NOW packet arrives. */
void onDataReceived(const uint8_t* mac, const uint8_t* data, int len) {

  memcpy(&inPacket, data, sizeof(inPacket));

  /* React only on button-pressed packets (sender sends on press edge only). */
  if (inPacket.button1State == BTN_PRESSED) {

    doorsOpen = !doorsOpen;
    int targetAngle = doorsOpen ? DOOR_OPEN_DEG : DOOR_CLOSE_DEG;

    servo1.write(targetAngle);
    servo2.write(targetAngle);

    Serial.printf("[INFO] Button press #%d → doors → %d°\n",
                  inPacket.pressCount, targetAngle);
  }
}


/* ── Setup ───────────────────────────────────────────────────────────────── */

void setup() {
  Serial.begin(115200);

  servo1.setPeriodHertz(SERVO_FREQ_HZ);
  servo2.setPeriodHertz(SERVO_FREQ_HZ);
  servo1.attach(SERVO1_PIN, SERVO_PULSE_MIN, SERVO_PULSE_MAX);
  servo2.attach(SERVO2_PIN, SERVO_PULSE_MIN, SERVO_PULSE_MAX);

  /* Move servos to their known starting positions. */
  servo1.write(DOOR_CLOSE_DEG);
  servo2.write(DOOR_CLOSE_DEG);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("[ERROR] ESP-NOW init failed — halting.");
    while (true) {}
  }

  esp_now_register_recv_cb(onDataReceived);

  Serial.println("[INFO] Receiver ready — waiting for packets.");
}


/* ── Main loop ───────────────────────────────────────────────────────────── */

void loop() {
  /* All logic is handled in the ESP-NOW callback. */
}
