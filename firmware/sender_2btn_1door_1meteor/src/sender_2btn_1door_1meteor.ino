/**
 * @file sender_2btn_1door_1meteor.ino
 * @brief ESP-NOW sender: two buttons independently control a door servo
 *        and a continuously-sweeping meteor servo on the receiver.
 *
 * Packet type : TwoButtonPacket
 * Matched with: receiver_2btn_1door_1meteor
 *
 * Wiring
 * ──────
 * Button 1 → GPIO 27 (INPUT_PULLUP)  — toggles the door
 * Button 2 → GPIO 25 (INPUT_PULLUP)  — toggles the meteor sweep
 */

#include <esp_now.h>
#include <WiFi.h>

#include "packet_types.h"
#include "constants.h"


/* ── Configuration ───────────────────────────────────────────────────────── */

/** MAC address of the matched receiver board. Update before flashing. */
static uint8_t RECEIVER_MAC[] = { 0x08, 0xB6, 0x1F, 0xEE, 0x62, 0xB8 };

static const int BOARD_ID    = 1;
static const int BUTTON1_PIN = 27;  // Door toggle
static const int BUTTON2_PIN = 25;  // Meteor toggle


/* ── State ───────────────────────────────────────────────────────────────── */

static TwoButtonPacket outPacket;
static esp_now_peer_info_t peerInfo;

static int lastBtn1 = BTN_NOT_PRESSED;
static int lastBtn2 = BTN_NOT_PRESSED;


/* ── Callbacks ───────────────────────────────────────────────────────────── */

/** Called after every ESP-NOW transmission; logs success or failure. */
void onDataSent(const uint8_t* macAddr, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "[ESP-NOW] Send OK" : "[ESP-NOW] Send FAILED");
}


/* ── Setup ───────────────────────────────────────────────────────────────── */

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

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
  int curBtn2 = digitalRead(BUTTON2_PIN);

  /* Only transmit when at least one button state has changed. */
  if (curBtn1 != lastBtn1 || curBtn2 != lastBtn2) {

    delay(DEBOUNCE_MS);

    int confirmedBtn1 = digitalRead(BUTTON1_PIN);
    int confirmedBtn2 = digitalRead(BUTTON2_PIN);

    /* Ignore noise: re-check that the change persisted. */
    if (confirmedBtn1 != lastBtn1 || confirmedBtn2 != lastBtn2) {

      outPacket.id           = BOARD_ID;
      outPacket.button1State = confirmedBtn1;
      outPacket.button2State = confirmedBtn2;

      esp_now_send(RECEIVER_MAC, (uint8_t*)&outPacket, sizeof(outPacket));

      lastBtn1 = confirmedBtn1;
      lastBtn2 = confirmedBtn2;
    }
  }

  delay(10);
}
