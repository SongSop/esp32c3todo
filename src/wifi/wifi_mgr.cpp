#include "wifi_mgr.h"
#include <Arduino.h>

const char* ssid = "Xiaomi_1BE7";
const char* password = "15170819789";

bool wifiConnect() {

  WiFi.begin(ssid, password);

  Serial.print("Connecting WiFi");

  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED) {

    Serial.print(".");
    delay(500);

    if (millis() - start > 15000) {
      Serial.println("\nWiFi Timeout");
      return false;
    }
  }

  Serial.println("\nWiFi OK");
  return true;
}
