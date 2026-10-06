#include <Arduino.h>
#include <time.h>

#include "NetworkManager.h"
#include "TelemetryManager.h"

namespace
{
// ประเทศไทยใช้เวลา UTC+7 และ firmwareVersion ใช้ยืนยันการอัปเดต OTA
constexpr long THAILAND_UTC_OFFSET_SECONDS = 7 * 60 * 60;
constexpr char FIRMWARE_VERSION[] = "ota-demo-1";
}

// ขอเวลาปัจจุบันจาก NTP และตั้งเขตเวลาเป็นประเทศไทย
void initializeNetworkTime()
{
  configTime(THAILAND_UTC_OFFSET_SECONDS, 0, "pool.ntp.org", "time.google.com");
  Serial.println("NTP synchronization requested (Thailand UTC+7).");
}

// อ่านเวลาและเซ็นเซอร์ จากนั้นจัดข้อมูล JSON แล้วส่งไป ThingsBoard
bool sendTelemetry()
{
  // ถ้า NTP ยังไม่พร้อม ให้ลองใหม่ในรอบถัดไป
  struct tm localTime;
  if (!getLocalTime(&localTime, 1000))
  {
    Serial.println("Waiting for NTP time synchronization...");
    return false;
  }

  // แปลงวันและเวลาให้อยู่ในรูปแบบที่ส่งเป็นข้อความได้
  char date[11];
  char timeText[9];
  strftime(date, sizeof(date), "%Y-%m-%d", &localTime);
  strftime(timeText, sizeof(timeText), "%H:%M:%S", &localTime);

  // อ่านเซ็นเซอร์ภายในชิป ESP32 รุ่นดั้งเดิม
  const float temperature = temperatureRead();
  const int magnetic = hallRead();

  Serial.print("Date (Thailand): ");
  Serial.println(date);
  Serial.print("Time (Thailand): ");
  Serial.println(timeText);
  Serial.print("ESP32 chip temperature (C): ");
  Serial.println(temperature, 2);
  Serial.print("Hall sensor raw value: ");
  Serial.println(magnetic);

  // สร้าง JSON สำหรับ ThingsBoard รวมเวอร์ชันไว้ใช้ตรวจ OTA
  char payload[192];
  snprintf(payload, sizeof(payload),
           "{\"temperature\":%.2f,\"magnetic\":%d,\"date\":\"%s\",\"time\":\"%s\",\"firmwareVersion\":\"%s\"}",
           temperature, magnetic, date, timeText, FIRMWARE_VERSION);
  Serial.print("Sending telemetry: ");
  Serial.println(payload);

  if (publishTelemetry(payload))
  {
    Serial.println("Telemetry sent.");
    return true;
  }

  Serial.println("Date/time telemetry publish failed.");
  delay(2000);
  return false;
}
