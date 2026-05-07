/**
 * @file receiver_2btn_1door_1meteor.ino
 * @brief ESP-NOW receiver: button 1 toggles a door servo; button 2 toggles
 *        both the door and a continuously-sweeping "meteor" servo, plus two
 *        status LEDs.
 *
 *        The meteor sweep runs as a FreeRTOS task pinned to core 0 so it
 *        never blocks the ESP-NOW callback on core 1.
 *
 * Packet type : TwoButtonPacket
 * Matched with: sender_2btn_1door_1meteor
 *
 * Wiring
 * ──────
 * Servo 1 (door)      → GPIO 33
 * Servo 2 (meteor)    → GPIO 22
 * White LED (door)    → GPIO 27
 * Blue LED  (meteor)  → GPIO 26
 */

#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>

#include "packet_types.h"
#include "constants.h"


/* ── Configuration ───────────────────────────────────────────────────────── */

static const int SERVO1_PIN = 33;
static const int SERVO2_PIN = 22;
static const int WHITE_LED  = 27;
static const int BLUE_LED   = 26;

/** Door travel range in degrees. Adjust to match physical hardware. */
static const int DOOR_OPEN_DEG  = 90;
static const int DOOR_CLOSE_DEG = 0;

/** Meteor sweep parameters. */
static const int  SWEEP_MIN_DEG  = 0;
static const int  SWEEP_MAX_DEG  = 100;
static const int  SWEEP_STEP_DEG = 2;    // Degrees moved per tick
static const int  SWEEP_TICK_MS  = 25;   // Delay between steps (lower = faster)
static const int  SWEEP_PAUSE_MS = 100;  // Poll interval while paused


/* ── State ───────────────────────────────────────────────────────────────── */

static TwoButtonPacket inPacket;

static Servo servo1;
static Servo servo2;

/** Shared flag: set/cleared from the ESP-NOW callback, read by sweep task. */
static volatile bool meteorRunning = true;

static int doorAngle = DOOR_CLOSE_DEG;

static int lastBtn1 = BTN_NOT_PRESSED;
static int lastBtn2 = BTN_NOT_PRESSED;


/* ── FreeRTOS task ───────────────────────────────────────────────────────── */

/**
 * Continuously sweeps servo 2 back and forth while meteorRunning is true.
 * Pinned to core 0; the ESP-NOW callback runs on core 1.
 */
void taskMeteorSweep(void* pvParameters) {
  int pos  = SWEEP_MIN_DEG;
  int step = SWEEP_STEP_DEG;

  for (;;) {
    if (meteorRunning) {
      servo2.write(pos);
      pos += step;

      if (pos >= SWEEP_MAX_DEG || pos <= SWEEP_MIN_DEG) {
        step = -step;
      }

      vTaskDelay(pdMS_TO_TICKS(SWEEP_TICK_MS));
    } else {
      vTaskDelay(pdMS_TO_TICKS(SWEEP_PAUSE_MS));
    }
  }
}


/* ── Helpers ─────────────────────────────────────────────────────────────── */

/**
 * Moves the door servo to @p angle, then detaches it after 600 ms to
 * eliminate post-move jitter. The detach runs in a short-lived RTOS task
 * so the callback is never blocked.
 */
void moveDoor(int angle) {
  servo1.attach(SERVO1_PIN, SERVO_PULSE_MIN, SERVO_PULSE_MAX);
  servo1.write(angle);

  xTaskCreate(
    [](void*) {
      vTaskDelay(pdMS_TO_TICKS(600));
      servo1.detach();
      vTaskDelete(NULL);
    },
    "DetachDoor", 2048, NULL, 1, NULL
  );
}


/* ── Callbacks ───────────────────────────────────────────────────────────── */

/** Called automatically whenever an ESP-NOW packet arrives. */
void onDataReceived(const uint8_t* mac, const uint8_t* data, int len) {

  memcpy(&inPacket, data, sizeof(inPacket));

  /* Button 1 rising-edge: toggle door only. */
  if (inPacket.button1State == BTN_PRESSED && lastBtn1 == BTN_NOT_PRESSED) {
    doorAngle = (doorAngle == DOOR_CLOSE_DEG) ? DOOR_OPEN_DEG : DOOR_CLOSE_DEG;
    moveDoor(doorAngle);
    digitalWrite(WHITE_LED, !digitalRead(WHITE_LED));
    Serial.printf("[INFO] Button 1 → door → %d°\n", doorAngle);
  }

  /* Button 2 rising-edge: toggle door AND meteor sweep. */
  if (inPacket.button2State == BTN_PRESSED && lastBtn2 == BTN_NOT_PRESSED) {
    doorAngle = (doorAngle == DOOR_CLOSE_DEG) ? DOOR_OPEN_DEG : DOOR_CLOSE_DEG;
    moveDoor(doorAngle);
    digitalWrite(WHITE_LED, !digitalRead(WHITE_LED));

    meteorRunning = !meteorRunning;
    digitalWrite(BLUE_LED, !meteorRunning);  // LED on when meteor is STOPPED

    Serial.printf("[INFO] Button 2 → door → %d° | meteor → %s\n",
                  doorAngle, meteorRunning ? "running" : "stopped");
  }

  lastBtn1 = inPacket.button1State;
  lastBtn2 = inPacket.button2State;
}


/* ── Setup ───────────────────────────────────────────────────────────────── */

void setup() {
  Serial.begin(115200);

  pinMode(WHITE_LED, OUTPUT);
  pinMode(BLUE_LED,  OUTPUT);

  servo1.setPeriodHertz(SERVO_FREQ_HZ);
  servo2.setPeriodHertz(SERVO_FREQ_HZ);
  servo1.attach(SERVO1_PIN, SERVO_PULSE_MIN, SERVO_PULSE_MAX);
  servo2.attach(SERVO2_PIN, SERVO_PULSE_MIN, SERVO_PULSE_MAX);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("[ERROR] ESP-NOW init failed — halting.");
    while (true) {}
  }

  esp_now_register_recv_cb(onDataReceived);

  /* Launch the meteor sweep task on core 0. */
  xTaskCreatePinnedToCore(
    taskMeteorSweep,
    "MeteorSweep",
    4096,
    NULL,
    1,
    NULL,
    0
  );

  Serial.println("[INFO] Receiver ready — waiting for packets.");
}


/* ── Main loop ───────────────────────────────────────────────────────────── */

void loop() {
  /* All logic is handled in the ESP-NOW callback and the sweep task. */
}
