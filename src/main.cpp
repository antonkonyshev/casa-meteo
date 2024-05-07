#include "main.h"

time_t measurement_period = 15000;  // Default measurement period (ms)
time_t last_measurement_performed = 0;
time_t last_time_sync_timestamp = 0;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    setupSerial();
    printHardwareInfo();

    #ifdef ESP32_DEVICE
    btStop();
    #endif

    setupPreferences();
    preferences_t* preferences = getPreferences();
    measurement_period = preferences->measurement_period * 1000;

    setupDisplay();
    setupBmp280();
    setupMq7();
    setupWifi();
    setupRTC();
    setupRouting();

    printMemoryInfo();

    digitalWrite(LED_PIN, LOW);

    loadSensorData();
}

void loop() {
    if (millis() - measurement_period > last_measurement_performed) {
        digitalWrite(LED_PIN, HIGH);
        last_measurement_performed = millis();
        measurement_t* measurement = loadSensorData();

        #ifdef ESP32_DEVICE
        ESP_LOGI("main",
            "Temperature: %.2f °C   |   Pressure: %.2f mmHg   |   Altitude: %.2f m   |   Pollution: %.2f mg/m3",
            measurement->temperature, measurement->pressure, measurement->altitude, measurement->pollution);
        #endif
        #ifdef ESP8266_DEVICE
        Serial.printf(
            "Temperature: %.2f °C   |   Pressure: %.2f mmHg   |   Altitude: %.2f m   |   Pollution: %.2f mg/m3\n",
            measurement->temperature, measurement->pressure, measurement->altitude, measurement->pollution);
        #endif

        if (measurement->timestamp > last_time_sync_timestamp + getPreferences()->time_sync_period) {
            last_time_sync_timestamp = measurement->timestamp;
            wifiKeepAlive();
            syncTime();
        }
        digitalWrite(LED_PIN, LOW);
    }
}