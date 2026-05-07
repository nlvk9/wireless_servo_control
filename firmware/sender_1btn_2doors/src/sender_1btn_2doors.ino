/**
 * @file sender_1btn_2doors.ino
 * @brief ESP-NOW sender: one button controls two door servos on the receiver.
 *        A cumulative press counter is included in every packet so the
 *        receiver can track absolute position state independently.
 *
 * Packet type : SingleButtonPacket
 * Matched with: receiver_1btn_2doors
 *
 * Wiring
 * ──────
 * Button 1 → GPIO 26 (INPUT_PULLUP)
 */

#include <esp_now.h>
#include <WiFi.h>

#include "packet_types.h"
#include "constants.h"


/* ── Configuration ───────────────────────────────────────────────────────── */

/** MAC address of the matched receiver board. Update before flashing. */
static uint8_t RECEIVER_MAC[] = { 0x08, 0xB6, 0x1F, 0xEF, 0x8F, 0x80 };

static const int BOARD_ID    = 1;
static const int BUTTON1_PIN = 26;


/* ── State ───────────────────────────────────────────────────────────────── */

static SingleButtonPacket outPacket;
static esp_now_peer_info_t peerInfo;

static int lastBtn1    = BTN_NOT_PRESSED;
static int pressCount  = 0;


/* ── Callbacks ───────────────────────────────────────────────────────────── */

/** Called after every ESP-NOW transmission; logs success or failure. */
void onDataSent(const uint8_t* macAddr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "[ESP-NOW] Send OK" : "[ESP-NOW] Send FAILED");
}


/* ── Setup ───────────────────────────────────────────────────────────────── */

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON1_PIN, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("[ERROR] ESP-NOW init failed — halting.");
    while (true) {}
  }

  esp_now_register_send_cb((esp_now_send_cb_t)onDataSent);

  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, RECEIVER_MAC, 6);
  peerInfo.channel = ESPNOW_CHANNEL;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("[ERROR] Failed to add ESP-NOW peer — halting.");
    while (true) {}
  }

  Serial.println("[INFO] Sender ready.");
}


/* ── Main loop ───────────────────────────────────────────────────────────── */

void loop() {
  int curBtn1 = digitalRead(BUTTON1_PIN);

  /* Detect the leading edge of a button press only. */
  if (curBtn1 == BTN_PRESSED && lastBtn1 == BTN_NOT_PRESSED) {

    delay(DEBOUNCE_MS);

    /* Confirm the press survived the debounce window. */
    if (digitalRead(BUTTON1_PIN) == BTN_PRESSED) {

      pressCount++;

      outPacket.id           = BOARD_ID;
      outPacket.button1State = curBtn1;
      outPacket.pressCount   = pressCount;

      esp_now_send(RECEIVER_MAC, (uint8_t*)&outPacket, sizeof(outPacket));

      Serial.printf("[INFO] Button pressed — total presses: %d\n", pressCount);
    }
  }

  lastBtn1 = curBtn1;
  delay(20);
}
