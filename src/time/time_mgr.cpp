#include "time_mgr.h"
#include <Arduino.h>
#include <time.h>

bool syncTime() {
  configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov"); // 东八区
  Serial.println("Waiting for time sync...");

  time_t now = time(nullptr);
  unsigned long start = millis();

  while (now < 1600000000) {  // 等待时间同步到最近几年
    delay(500);
    Serial.print(".");

    now = time(nullptr);

    // 超时保护 15 秒
    if (millis() - start > 15000) {
      Serial.println("\nTime sync Timeout");
      return false;
    }
  }

  Serial.println();
  Serial.print("Current UNIX time: ");
  Serial.println(now);

  return true;
}

time_t getUnixTime() {
  return time(nullptr);
}
