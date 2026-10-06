#pragma once

// เข้า Deep Sleep ตามเวลาที่กำหนดใน PowerManager.cpp
// ปัจจุบัน main.cpp ยังไม่เรียกใช้ เพื่อให้ OTA พร้อมทำงานตลอดเวลา
void enterDeepSleep();
