#include "main.h"

hw_timer_t* measurement_timer = NULL;
bool perform_periodical_measurement = false;
bool periodical_measurements_since_last_time_sync = 0;

void performPeriodicalMeasurement() {
    perform_periodical_measurement = true;
}

void setupTimers() {
    measurement_timer = timerBegin(0, 8000, true);
    timerAttachInterrupt(measurement_timer, &performPeriodicalMeasurement, true);
    timerAlarmWrite(measurement_timer, getPreferences()->measurement_period * 10000, true);
    timerAlarmEnable(measurement_timer);
}

void setup() {
    Serial.begin(9600);
    delay(100);
    Serial.println("");
    Serial.println("--------------------------------- Meteo ---------------------------------");

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    btStop();
    setupPreferences();
    setupDisplay();
    setupBmp280();
    setupMq7();
    setupWifi();
    setupRTC();
    setupRouting();
    setupTimers();

    Serial.println("");
    Serial.printf("Total  heap: %8d bytes     |     Free  heap: %8d bytes\n", ESP.getHeapSize(), ESP.getFreeHeap());
    Serial.printf("Total PSRAM: %8d bytes     |     Free PSRAM: %8d bytes\n", ESP.getPsramSize(), ESP.getFreePsram());
    Serial.printf("Sketch size: %8d bytes     |     Free space: %8d bytes\n", ESP.getSketchSize(), ESP.getFreeSketchSpace());
    Serial.println("");

    digitalWrite(LED_PIN, LOW);
}

void loop() {
//    getServer()->handleClient();
    if (perform_periodical_measurement) {
        digitalWrite(LED_PIN, HIGH);
        measurement_t* measurement = loadSensorData();
        ESP_LOGI("main",
            "Temperature: %.2f °C   |   Pressure: %.2f mmHg   |   Altitude: %.2f m   |   Pollution: %.2f mg/m3",
            measurement->temperature, measurement->pressure, measurement->altitude, measurement->pollution);
        perform_periodical_measurement = false;
        periodical_measurements_since_last_time_sync += 1;
        if (periodical_measurements_since_last_time_sync >= getPreferences()->time_sync_periodicity) {
            periodical_measurements_since_last_time_sync = 0;
            wifiKeepAlive();
            configTime(0, 0, NTP_SERVER_1, NTP_SERVER_2, NTP_SERVER_3);
        }
        digitalWrite(LED_PIN, LOW);
    }
}