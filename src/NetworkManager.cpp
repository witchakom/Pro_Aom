#include <Arduino.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "NetworkManager.h"

namespace
{
// ค่าการเชื่อมต่อเครือข่ายและบัญชีอุปกรณ์
const char *WIFI_SSID = "OPPO A92JR";
const char *WIFI_PASSWORD = "0000000000";
const char *THINGSBOARD_SERVER = "mqtt.thingsboard.cloud";
constexpr uint16_t THINGSBOARD_PORT = 1883;
const char *THINGSBOARD_ACCESS_TOKEN = "oZPfzPESC92CHFAxZHdZ";
const char *OTA_PASSWORD = "123456789";

// เก็บ client ไว้ใช้ร่วมกันระหว่างฟังก์ชันในไฟล์นี้
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
}

// ตรวจสถานะ Wi-Fi ปัจจุบัน
bool isWiFiConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

// เชื่อมต่อ Wi-Fi และรอจนกว่าจะได้รับ IP address
void connectWiFi()
{
  Serial.println("Connecting to Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (!isWiFiConnected())
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// เริ่ม OTA และตั้ง callback สำหรับรายงานสถานะการอัปโหลด
void startOtaService()
{
  ArduinoOTA.setHostname("esp32-project-aom");
  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.onStart([]() { Serial.println("OTA update started."); });
  ArduinoOTA.onEnd([]() { Serial.println("OTA update finished; restarting."); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
    Serial.printf("OTA progress: %u%%\r", (progress * 100U) / total);
  });
  ArduinoOTA.onError([](ota_error_t error)
  {
    Serial.printf("OTA error [%u].\n", static_cast<unsigned int>(error));
  });

  ArduinoOTA.begin();
  Serial.print("OTA ready at IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("OTA service is active while the board stays awake.");
}

// ประมวลผลคำขอ OTA; ต้องเรียกซ้ำจาก loop() ขณะบอร์ดทำงาน
void serviceOta()
{
  ArduinoOTA.handle();
}

// เชื่อมต่อ MQTT กับ ThingsBoard โดยใช้ access token เป็นรหัสผ่าน
void connectThingsBoard()
{
  mqttClient.setServer(THINGSBOARD_SERVER, THINGSBOARD_PORT);

  while (!mqttClient.connected())
  {
    Serial.print("Connecting to ThingsBoard MQTT...");
    if (mqttClient.connect("ESP32", THINGSBOARD_ACCESS_TOKEN, nullptr))
    {
      Serial.println(" connected!");
    }
    else
    {
      Serial.print(" failed, state = ");
      Serial.println(mqttClient.state());
      Serial.println("Retrying in 2 seconds...");
      delay(2000);
    }
  }
}

// ตรวจสอบสถานะ MQTT
bool isThingsBoardConnected()
{
  return mqttClient.connected();
}

// ให้ PubSubClient ดูแลงาน MQTT ที่ค้างอยู่
void serviceThingsBoard()
{
  mqttClient.loop();
}

// ส่งข้อความ JSON ไปยัง topic telemetry มาตรฐานของ ThingsBoard
bool publishTelemetry(const char *payload)
{
  return mqttClient.publish("v1/devices/me/telemetry", payload);
}
