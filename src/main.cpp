#include <Arduino.h>
#include "NetworkManager.h"
#include "TelemetryManager.h"

namespace
{
// ส่ง telemetry ทุก 5 นาที โดยแปลงหน่วยเป็นมิลลิวินาที
constexpr uint32_t TELEMETRY_INTERVAL_MS = 5UL * 60UL * 1000UL;
}

// ทำงานครั้งเดียวตอนเปิดหรือรีเซ็ตบอร์ด
void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("ESP32 NTP time and ThingsBoard MQTT");

  // เริ่มการเชื่อมต่อและบริการต่าง ๆ ก่อนเข้าลูปหลัก
  connectWiFi();
  startOtaService();
  initializeNetworkTime();
  connectThingsBoard();
}

// ทำงานวนซ้ำตลอดเวลา เพื่อให้ OTA พร้อมรับคำสั่งอยู่เสมอ
void loop()
{
  // ต้องเรียก handler เป็นประจำ ไม่เช่นนั้น OTA จะรับการอัปเดตไม่ได้
  serviceOta();

  if (!isWiFiConnected())
  {
    delay(100);
    return;
  }

  // หาก MQTT หลุด ให้เชื่อมต่อ ThingsBoard ใหม่
  if (!isThingsBoardConnected())
    connectThingsBoard();
  serviceThingsBoard();

  // ส่งครั้งแรกทันทีเมื่อพร้อม จากนั้นส่งซ้ำตามช่วงเวลาที่กำหนด
  static bool firstTelemetrySent = false;
  static uint32_t lastTelemetryTime = 0;
  const uint32_t now = millis();

  if (!firstTelemetrySent || now - lastTelemetryTime >= TELEMETRY_INTERVAL_MS)
  {
    if (sendTelemetry())
    {
      firstTelemetrySent = true;
      lastTelemetryTime = millis();
      Serial.println("Deep Sleep is disabled; OTA stays available.");
      Serial.println("Next telemetry is scheduled in 5 minutes.");
    }
  }

  // ปิด Deep Sleep ไว้ เพื่อให้ Wi-Fi และ OTA ทำงานต่อเนื่อง
  // หากเปิดใช้ Deep Sleep บอร์ดจะออฟไลน์และรับ OTA ไม่ได้ระหว่างหลับ
  // delay(PRE_SLEEP_DELAY_SECONDS * 1000UL);
  // enterDeepSleep();
  delay(10);
}
