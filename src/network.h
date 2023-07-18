#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESPAsyncWebServer.h>

#include "pinout.h"
#include "sensors.h"

#define API_PORT 80

#define WIFI_CONNECTION_TIMEOUT 60  // seconds
#define RTC_SYNC_TIMEOUT 60  // seconds

#define NTP_SERVER_1 "pool.ntp.org"
#define NTP_SERVER_2 "time.google.com"
#define NTP_SERVER_3 "time.cloudflare.com"

void wifiKeepAlive();
void setupRouting();
bool setupWifi();
bool setupRTC();
AsyncWebServer* getServer();