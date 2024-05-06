#include "server.h"

AsyncWebServer server(API_PORT);

void setupRouting() {
    server.on("/", [](AsyncWebServerRequest* request) {
        digitalWrite(LED_PIN, HIGH);
        request->send(200, "application/json", getLastMeasurementSerialized());
        digitalWrite(LED_PIN, LOW);
    });

    server.on("/journal", [](AsyncWebServerRequest* request) {
        digitalWrite(LED_PIN, HIGH);
        journal_t* journal = getJournal();
        if (!journal->first) {
            request->send(200, "application/json", "[]");
            return;
        }
        record_t* record = journal->first;
        AsyncResponseStream* response = request->beginResponseStream("application/json");
        response->print("[");
        while (record) {
            if (record != journal->first) {
                response->print(",");
            }
            response->print(record->message);
            record = record->next;
        }
        response->print("]");
        request->send(response);
        digitalWrite(LED_PIN, LOW);
    });

    server.on("/service", [](AsyncWebServerRequest* request) {
        digitalWrite(LED_PIN, HIGH);
        // The service endpoint response is a constant for the service, since it doesn't changes within time while the device is working
        request->send(200, "application/json", "{\"service\":\"meteo\",\"name\":\"Room\",\"id\":\"country-house-room-1\",\"sensors\":[\"temperature\",\"pressure\",\"altitude\",\"pollution\"]}");
        digitalWrite(LED_PIN, LOW);
    });

    server.on("/settings", HTTP_POST, [](AsyncWebServerRequest* request) {
        digitalWrite(LED_PIN, HIGH);
        preferences_t* preferences = getPreferences();
        size_t params = request->params();
        for (size_t i = 0; i < params; i++) {
            AsyncWebParameter* param = request->getParam(i);
            if (param->name() == "high_pollution_value") {
                preferences->high_pollution_value = param->value().toInt();
            } else if (param->name() == "min_thermometer_temperature") {
                preferences->min_thermometer_temperature = param->value().toInt();
            } else if (param->name() == "max_thermometer_temperature") {
                preferences->max_thermometer_temperature = param->value().toInt();
            } else if (param->name() == "measurement_period") {
                preferences->measurement_period = param->value().toInt();
            } else if (param->name() == "time_sync_period") {
                preferences->time_sync_period = param->value().toInt();
            } else if (param->name() == "journal_length") {
                preferences->journal_length = param->value().toInt();
            } else if (param->name() == "history_record_period") {
                preferences->history_record_period = param->value().toInt();
            }
        }
        request->send(200);
        saveSettings(preferences);
        digitalWrite(LED_PIN, LOW);
    });

    server.on("/settings", [](AsyncWebServerRequest* request) {
        digitalWrite(LED_PIN, HIGH);
        preferences_t* preferences = getPreferences();
        char buffer[512] = {0};
        snprintf(buffer, 512,
            "{\"high_pollution_value\":%d,\"min_thermometer_temperature\":%d,\"max_thermometer_temperature\":%d,\"measurement_period\":%d,\"time_sync_period\":%d,\"journal_length\":%d,\"history_record_period\":%d,\"wifi_ssid\":\"%s\"}",
            preferences->high_pollution_value, preferences->min_thermometer_temperature,
            preferences->max_thermometer_temperature, preferences->measurement_period,
            preferences->time_sync_period, preferences->journal_length,
            preferences->history_record_period, preferences->wifi_ssid);
        request->send(200, "application/json", buffer);
        digitalWrite(LED_PIN, LOW);
    });

    server.begin();
}