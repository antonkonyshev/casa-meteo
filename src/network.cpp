#include "network.h"

WiFiMulti wifiMulti;
AsyncWebServer server(API_PORT);
bool wifiApMode = false;

void setupRouting() {
    server.on("/", [](AsyncWebServerRequest* request) {
        digitalWrite(LED_PIN, HIGH);
        request->send(200, "application/json", getLastMeasurementSerialized());
        digitalWrite(LED_PIN, LOW);
    });

    server.on("/history", [](AsyncWebServerRequest* request) {
        digitalWrite(LED_PIN, HIGH);
        history_t* history = getHistory();
        if (!history->first) {
            request->send(200, "application/json", "[]");
            return;
        }
        history_record_t* record = history->first;
        AsyncResponseStream* response = request->beginResponseStream("application/json");
        response->print("[");
        while (record) {
            if (record != history->first) {
                response->print(",");
            }
            response->print(record->measurement.c_str());
            record = record->next;
        }
        response->print("]");
        request->send(response);
        digitalWrite(LED_PIN, LOW);
    });

    // TODO: Move device ID to preferences and make it editable
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
        for (int i = 0; i < params; i++) {
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
            } else if (param->name() == "history_length") {
                preferences->history_length = param->value().toInt();
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
            "{\"high_pollution_value\":%d,\"min_thermometer_temperature\":%d,\"max_thermometer_temperature\":%d,\"measurement_period\":%d,\"time_sync_period\":%d,\"history_length\":%d,\"history_record_period\":%d,\"wifi_ssid\":\"%s\"}",
            preferences->high_pollution_value, preferences->min_thermometer_temperature,
            preferences->max_thermometer_temperature, preferences->measurement_period,
            preferences->time_sync_period, preferences->history_length,
            preferences->history_record_period, preferences->wifi_ssid.c_str());
        request->send(200, "application/json", buffer);
        digitalWrite(LED_PIN, LOW);
    });

    server.begin();
}

void indicateSOS() {
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    for (uint8_t idx = 0; idx < 3; idx++) {
        digitalWrite(LED_PIN, HIGH);
        delay(200);
        digitalWrite(LED_PIN, LOW);
        delay(200);
    }
    delay(600);
    for (uint8_t idx = 0; idx < 3; idx++) {
        digitalWrite(LED_PIN, HIGH);
        delay(600);
        digitalWrite(LED_PIN, LOW);
        delay(600);
    }
    delay(600);
    for (uint8_t idx = 0; idx < 3; idx++) {
        digitalWrite(LED_PIN, HIGH);
        delay(200);
        digitalWrite(LED_PIN, LOW);
        delay(200);
    }
    delay(1000);
}

void wifiKeepAlive() {
    if (wifiApMode) {
        indicateSOS();
        return;
    }
    uint8_t ledState = LOW;
    uint8_t retries = 0;
    while (wifiMulti.run(WIFI_CONNECTION_TIMEOUT * 1000) != WL_CONNECTED) {
        ledState = ledState == LOW ? HIGH : LOW;
        digitalWrite(LED_PIN, ledState);
        delay(1000);
        retries += 1;
        if (retries > WIFI_STA_CONNECT_RETRIES) {
            break;
        }
    }
    if (retries > WIFI_STA_CONNECT_RETRIES) {
        WiFi.mode(WIFI_MODE_AP);
        WiFi.softAP("home_esp_country_house_room_1");
        ESP_LOGI("home", "Couldn't connect to any available WiFi network. Fallback to access point mode with ssid 'home_esp_.*'. Please, connect to configure the device. After the configuration a reboot will be necessary.");
        wifiApMode = true;
        indicateSOS();
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
