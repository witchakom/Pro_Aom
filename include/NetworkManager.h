#pragma once

// Wi-Fi
bool isWiFiConnected();
void connectWiFi();

// OTA
void startOtaService();
void serviceOta();

// ThingsBoard ผ่าน MQTT
void connectThingsBoard();
bool isThingsBoardConnected();
void serviceThingsBoard();
bool publishTelemetry(const char *payload);
