#include <Arduino.h>
#include <WiFi.h>
#include "esp_sleep.h"

#include "PowerManager.h"

namespace
{
// ระยะเวลาพักของบอร์ดเมื่อมีการเรียกใช้ Deep Sleep
constexpr uint32_t SLEEP_DURATION_MINUTES = 5;
}

// ฟังก์ชันนี้ยังเก็บไว้ แต่ main.cpp ปิดการเรียกใช้เพื่อให้ OTA ออนไลน์ตลอด
void enterDeepSleep()
{
  const uint64_t sleepTimeUs =
      static_cast<uint64_t>(SLEEP_DURATION_MINUTES) * 60ULL * 1000000ULL;

  Serial.printf("Entering deep sleep for %lu minute(s).\n",
                static_cast<unsigned long>(SLEEP_DURATION_MINUTES));
  Serial.flush();

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  esp_sleep_enable_timer_wakeup(sleepTimeUs);
  esp_deep_sleep_start();
}
