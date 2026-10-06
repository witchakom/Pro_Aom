#pragma once

#include <Arduino.h>

// ตั้งค่าเวลา NTP และส่ง telemetry หนึ่งชุด
void initializeNetworkTime();
bool sendTelemetry();
