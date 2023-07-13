#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebServer.h>
#define ARDUINOJSON_USE_DOUBLE 0
#include <ArduinoJson.h>

#include "sensors.h"
#include "preferences.h"

#define API_PORT 80

#define WIFI_CONNECTION_TIMEOUT 60  // seconds
#define RTC_SYNC_TIMEOUT 60  // seconds
#define SERIALIZED_MEASUREMENT_MAX_LENGTH 512    // near 245 bytes per measurement

#define NTP_SERVER_1 "pool.ntp.org"
#define NTP_SERVER_2 "time.google.com"
#define NTP_SERVER_3 "time.cloudflare.com"

void wifiKeepAlive();
void setupRouting();
bool setupWifi();
bool setupRTC();
WebServer* getServer();