#include "network.h"
#include "pinout.h"

WiFiMulti wifiMulti;

WebServer server(API_PORT);

void createJson(measurement_t* measurement, JsonObject* json_object) {
    json_object->operator[]("timestamp") = measurement->timestamp;
    JsonObject obj = json_object->createNestedObject("temperature");
    obj["value"] = measurement->temperature;
    obj["unit"] = "°C";
    obj = json_object->createNestedObject("pressure");
    obj["value"] = measurement->pressure;
    obj["unit"] = "mmHg";
    obj = json_object->createNestedObject("altitude");
    obj["value"] = measurement->altitude;
    obj["unit"] = "m";
    obj = json_object->createNestedObject("pollution");
    obj["value"] = measurement->pollution;
    obj["unit"] = "mgm3";
}

void getServiceInfoResponse() {
    digitalWrite(LED_PIN, HIGH);
    StaticJsonDocument<256> json_document;
    char buffer[256] = { '\0' };
    json_document["service"] = "meteo";
    json_document["name"] = "Room";
    JsonArray sensors = json_document.createNestedArray("sensors");
    sensors.add("temperature");
    sensors.add("pressure");
    sensors.add("altitude");
    sensors.add("pollution");
    serializeJson(json_document, buffer);
    server.send(200, "application/json", buffer);
    digitalWrite(LED_PIN, LOW);
}

void getMeteoResponse() {
    digitalWrite(LED_PIN, HIGH);
    measurement_t* measurement = loadSensorData();
    StaticJsonDocument<SERIALIZED_MEASUREMENT_MAX_LENGTH> json_document;
    JsonObject json_object = json_document.to<JsonObject>();
    createJson(measurement, &json_object);
    char buffer[SERIALIZED_MEASUREMENT_MAX_LENGTH];
    serializeJson(json_document, buffer);
    server.send(200, "application/json", buffer);
    digitalWrite(LED_PIN, LOW);
}

void getHistoryResponse() {
    digitalWrite(LED_PIN, HIGH);
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "application/json", "[");
    history_t* history = getHistory();
    if (history->first) {
        char buffer[SERIALIZED_MEASUREMENT_MAX_LENGTH] = { '\0' };
        history_record_t* record = history->first;
        while (record) {
            memset(&buffer[0], 0, sizeof(buffer));
            StaticJsonDocument<SERIALIZED_MEASUREMENT_MAX_LENGTH> json_document;
            JsonObject json_object = json_document.to<JsonObject>();
            createJson(record->measurement, &json_object);
            if (record != history->first) {
                server.sendContent(",");
            }
            serializeJson(json_document, buffer);
            server.sendContent(buffer);
            record = record->next;
        }
    }
    server.sendContent("]");
    server.sendContent("");
    server.client().stop();
    digitalWrite(LED_PIN, LOW);
}

void setupRouting() {
    server.on("/", getMeteoResponse);
    server.on("/service", getServiceInfoResponse);
    server.on("/history", getHistoryResponse);
    server.begin();
}

void wifiKeepAlive() {
    uint8_t ledState = LOW;
    while (wifiMulti.run(WIFI_CONNECTION_TIMEOUT * 1000) != WL_CONNECTED) {
        ledState = ledState == LOW ? HIGH : LOW;
        digitalWrite(LED_PIN, ledState);
        delay(100);
    }
    digitalWrite(LED_PIN, LOW);
}

bool setupWifi() {
    WiFi.mode(WIFI_MODE_STA);
    wifi_credentials_t* credentials = loadWiFiCredentials();
    while (credentials) {
        wifiMulti.addAP(credentials->ssid.c_str(), credentials->password.c_str());
        credentials = credentials->next;
    }
    cleanWiFiCredentials();
    wifiKeepAlive();
    digitalWrite(LED_PIN, HIGH);
    return true;
}

bool setupRTC() {
    time_t now;
    configTime(0, 0, NTP_SERVER_1, NTP_SERVER_2, NTP_SERVER_3);
    bool ledState = LOW;
    time_t timeout = millis() + RTC_SYNC_TIMEOUT * 1000;
    bool result = true;
    while (now < 1687100000) {
        digitalWrite(LED_PIN, ledState);
        ledState = ledState == LOW ? HIGH : LOW;
        time(&now);
        delay(100);
        if (millis() > timeout) {
            result = false;
            break;
        }
    }
    digitalWrite(LED_PIN, HIGH);
    return result;
}

WebServer* getServer() {
    return &server;
}