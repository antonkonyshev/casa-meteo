#include "preferences.h"

preferences_t* preferences;

void saveSettings(preferences_t* prefs) {
    #ifdef ESP32_DEVICE
    openPreferencesStorage();

    savePreference(NVS_HIGH_POLLUTION_VALUE_KEY, prefs->high_pollution_value);
    savePreference(NVS_MIN_THERMOMETER_TEMPERATURE_KEY, prefs->min_thermometer_temperature);
    savePreference(NVS_MAX_THERMOMETER_TEMPERATURE_KEY, prefs->max_thermometer_temperature);
    savePreference(NVS_MEASUREMENT_PERIOD_KEY, prefs->measurement_period);
    savePreference(NVS_TIME_SYNC_PERIOD_KEY, prefs->time_sync_period);
    savePreference(NVS_JOURNAL_LENGTH_KEY, prefs->journal_length);
    savePreference(NVS_HISTORY_RECORD_PERIOD_KEY, prefs->history_record_period);
    if (prefs->wifi_ssid) {
        savePreference(NVS_WIFI_SSID_CREDENTIALS_KEY, prefs->wifi_ssid);
        savePreference(NVS_WIFI_PASSWORD_CREDENTIALS_KEY, prefs->wifi_password);
    }

    closePreferencesStorage();
    #endif
    #ifdef ESP8266_DEVICE
    savePreference(prefs);
    if (prefs->wifi_ssid) {
        saveWifiCredentials(prefs->wifi_ssid, prefs->wifi_password);
    }
    #endif
}

void saveSettings(AsyncWebServerRequest* request) {
    size_t params = request->params();
    for (size_t idx = 0; idx < params; idx++) {
        AsyncWebParameter* param = request->getParam(idx);

        if (param->name() == NVS_HIGH_POLLUTION_VALUE_KEY) {
            preferences->high_pollution_value = param->value().toInt();
        } else if (param->name() == NVS_MIN_THERMOMETER_TEMPERATURE_KEY) {
            preferences->min_thermometer_temperature = param->value().toInt();
        } else if (param->name() == NVS_MAX_THERMOMETER_TEMPERATURE_KEY) {
            preferences->max_thermometer_temperature = param->value().toInt();
        } else if (param->name() == NVS_MEASUREMENT_PERIOD_KEY) {
            preferences->measurement_period = param->value().toInt();
        } else if (param->name() == NVS_TIME_SYNC_PERIOD_KEY) {
            preferences->time_sync_period = param->value().toInt();
        } else if (param->name() == NVS_JOURNAL_LENGTH_KEY) {
            preferences->journal_length = param->value().toInt();
        } else if (param->name() == NVS_HISTORY_RECORD_PERIOD_KEY) {
            preferences->history_record_period = param->value().toInt();
        } else if (param->name() == NVS_WIFI_SSID_CREDENTIALS_KEY) {
            preferences->wifi_ssid = param->value().c_str();
        } else if (param->name() == NVS_WIFI_PASSWORD_CREDENTIALS_KEY) {
            preferences->wifi_password = param->value().c_str();
        }
    }
    saveSettings(preferences);
}

void setupPreferences() {
    preferences = new preferences_t;

    #ifdef ESP32_DEVICE
    openPreferencesStorage();

    loadPreference(NVS_HIGH_POLLUTION_VALUE_KEY, &preferences->high_pollution_value, (uint16_t) NVS_HIGH_POLLUTION_VALUE_DEFAULT);
    loadPreference(NVS_MIN_THERMOMETER_TEMPERATURE_KEY, &preferences->min_thermometer_temperature, NVS_MIN_THERMOMETER_TEMPERATURE_DEFAULT);
    loadPreference(NVS_MAX_THERMOMETER_TEMPERATURE_KEY, &preferences->max_thermometer_temperature, NVS_MAX_THERMOMETER_TEMPERATURE_DEFAULT);
    loadPreference(NVS_MEASUREMENT_PERIOD_KEY, &preferences->measurement_period, (uint16_t) NVS_MEASUREMENT_PERIOD_DEFAULT);
    loadPreference(NVS_TIME_SYNC_PERIOD_KEY, &preferences->time_sync_period, (uint16_t) NVS_TIME_SYNC_PERIOD_DEFAULT);
    loadPreference(NVS_JOURNAL_LENGTH_KEY, &preferences->journal_length, (uint16_t) NVS_JOURNAL_LENGTH_DEFAULT);
    loadPreference(NVS_HISTORY_RECORD_PERIOD_KEY, &preferences->history_record_period, (uint16_t) NVS_HISTORY_RECORD_PERIOD_DEFAULT);
    
    closePreferencesStorage();
    #endif
    #ifdef ESP8266_DEVICE
    if (!loadPreference(preferences)) {
        preferences->journal_length = NVS_JOURNAL_LENGTH_DEFAULT;
    }
    #endif
}

void serializeSettings(char* buffer) {
    sprintf(buffer,
        "{\"high_pollution_value\":%d,\"min_thermometer_temperature\":%d,\"max_thermometer_temperature\":%d,\"measurement_period\":%d,\"time_sync_period\":%d,\"journal_length\":%d,\"history_record_period\":%d}",
        preferences->high_pollution_value, preferences->min_thermometer_temperature,
        preferences->max_thermometer_temperature, preferences->measurement_period,
        preferences->time_sync_period, preferences->journal_length,
        preferences->history_record_period);
}

preferences_t* getPreferences() {
    return preferences;
}
