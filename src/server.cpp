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
        request->send(200, "application/json", "{\"service\":\"meteo\",\"name\":\"Room\",\"id\":\"" + String(DEVICE_ID) + "\",\"sensors\":[\"temperature\",\"pressure\",\"altitude\",\"pollution\"]}");
        digitalWrite(LED_PIN, LOW);
    });

    server.on("/settings", HTTP_POST, [](AsyncWebServerRequest* request) {
        digitalWrite(LED_PIN, HIGH);
        saveSettings(request);
        request->send(200);
        digitalWrite(LED_PIN, LOW);
    });

    server.on("/settings", [](AsyncWebServerRequest* request) {
        digitalWrite(LED_PIN, HIGH);
        char payload[512] = {0};
        serializeSettings(payload);
        request->send(200, "application/json", (const char*) payload);
        digitalWrite(LED_PIN, LOW);
    });

    server.begin();
}