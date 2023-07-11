#include "main.h"
#include "pinout.h"
#include "network.h"
#include "display.h"

// TODO: Current time on the display
// TODO: Pollution indicator on the display
// TODO: Pressure on the display
// TODO: Wifi ssid and password configuration via bluetooth
// TODO: Use average value for pollution
// TODO: Periodical time sync via NTP
// TODO: Measure offten, but write to history only every hour

hw_timer_t* measurement_timer = NULL;
bool perform_periodical_measurement = false;

void performPeriodicalMeasurement() {
    perform_periodical_measurement = true;
}

void setup() {
    Serial.begin(9600);
    Serial.println("");
    Serial.println("--------------------------------- Meteo ---------------------------------");

    Serial.print("Initializing LED indication ...");
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    Serial.println(" [ Done ]");

    Serial.print("Disabling Bluetooth module ...");
    if (btStop()) {
        Serial.println(" [ Done ]");
    } else {
        Serial.print(" [ Fail ]");
    }

    Serial.print("Initializing TFT display ...");
    if (setupDisplay()) {
        Serial.println(" [ Done ]");
    } else {
        Serial.println(" [ Fail ]");
    }

    Serial.print("Initializing BMP280 sensor ...");
    if (setupBmp280()) {
        Serial.println(" [ Done ]");
    } else {
        Serial.println(" [ Fail ]");
    }

    Serial.print("Initializing MQ-7 sensor ...");
    if (setupMq7()) {
        Serial.println(" [ Done ]");
    } else {
        Serial.println(" [ Fail ]");
    }

    Serial.print("Connecting to WiFi network ...");
    if (setupWifi()) {
      Serial.println(" [ Done ]");
    } else {
      Serial.println(" [ Fail ]");
    }

    Serial.print("Setting up the real time clock ...");
    if (setupRTC()) {
      Serial.println(" [ Done ]");
    } else {
      Serial.println(" [ Fail ]");
    }

    Serial.print("Starting HTTP API server ...");
    setupRouting();
    Serial.println(" [ Done ]");

    Serial.print("Setting up timers ...");
    measurement_timer = timerBegin(0, 8000, true);
    timerAttachInterrupt(measurement_timer, &performPeriodicalMeasurement, true);
    timerAlarmWrite(measurement_timer, MEASUREMENT_PERIOD * 10000, true);
    timerAlarmEnable(measurement_timer);
    Serial.println(" [ Done ]");

    Serial.print("Local IP address: ");
    Serial.println(WiFi.localIP());

    Serial.printf("Total    heap: %8d bytes     |     Free    heap: %8d bytes\n", ESP.getHeapSize(), ESP.getFreeHeap());
    Serial.printf("Total PSRAM: %8d bytes     |     Free PSRAM: %8d bytes\n", ESP.getPsramSize(), ESP.getFreePsram());
    Serial.printf("Sketch size: %8d bytes     |     Free space: %8d bytes\n", ESP.getSketchSize(), ESP.getFreeSketchSpace());

    Serial.println("");
    loadSensorData();
    digitalWrite(LED_PIN, LOW);
}

void loop() {
    getServer()->handleClient();
    if (perform_periodical_measurement) {
        digitalWrite(LED_PIN, HIGH);
        measurement_t* measurement = loadSensorData();
        Serial.print("Temperature: ");
        Serial.print(measurement->temperature);
        Serial.print(" °C     |     Pressure: ");
        Serial.print(measurement->pressure);
        Serial.print(" mmHg     |     Altitude: ");
        Serial.print(measurement->altitude);
        Serial.print(" m     |     Pollution: ");
        Serial.print(measurement->pollution);
        Serial.println(" mgm3");
        perform_periodical_measurement = false;
        digitalWrite(LED_PIN, LOW);
    }
}