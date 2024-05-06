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
    loadPreference(preferences);
    #endif
}

preferences_t* getPreferences() {
    return preferences;
}
