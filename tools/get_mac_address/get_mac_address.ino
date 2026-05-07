/**
 * @file get_mac_address.ino
 * @brief Utility sketch: prints this board's Wi-Fi MAC address to the
 *        Serial Monitor in the format expected by RECEIVER_MAC[] arrays.
 *
 * Flash this to a receiver board, open Serial Monitor at 115200 baud,
 * then copy the printed address into the matching sender's source file.
 *
 * Usage
 * ─────
 * 1. Flash to the receiver ESP32.
 * 2. Open Serial Monitor at 115200 baud.
 * 3. Copy the printed address into the sender:
 *       static uint8_t RECEIVER_MAC[] = { 0xAA, 0xBB, ... };
 */

#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);

  uint8_t mac[6];
  esp_read_mac(mac, ESP_MAC_WIFI_STA);

  Serial.println("──────────────────────────────────");
  Serial.println("  ESP32 Wi-Fi Station MAC Address");
  Serial.println("──────────────────────────────────");

  /* Human-readable format */
  Serial.printf("  %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  /* C array literal — paste directly into sender source */
  Serial.printf("  { 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X }\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  Serial.println("──────────────────────────────────");
}

void loop() {}
