#include "server.h"

AsyncWebServer server(API_PORT);

void serviceInfoResponsePayload(char* buffer) {
    sprintf(buffer,
        "{\"service\":\"meteo\",\"name\":\"Room\",\"id\":\"%s\",\"sensors\":[\"temperature\",\"pressure\",\"altitude\",\"pollution\"]}",
        DEVICE_ID);
}

void setupRouting() {
    server.on("/", [](AsyncWebServerRequest* request) {
        digitalWrite(LED_PIN, HIGH);
        request->send(200, "application/json", getLastMeasurementSerialized());
        digitalWrite(LED_PIN, LOW);
    });

    server.on("/journal", handleJournalRequest);
    server.on("/service", handleServiceInfoRequest);
    server.on("/settings", HTTP_POST, handleSettingsEditRequest);
    server.on("/settings", handleSettingsReadRequest);

    server.begin();
}