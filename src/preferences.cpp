#include "preferences.h"

wifi_credentials_t* credentials;
preferences_t* preferences;

void appendToCredentials(const char* ssid, const char* password) {
    wifi_credentials_t* additional = new wifi_credentials_t;
    additional->ssid = ssid;
    additional->password = password;
    if (credentials) {
        wifi_credentials_t* current = credentials;
        while(current->next) {
            current = current->next;
        }
        current->next = additional;
    } else {
        credentials = additional;
    }
}

wifi_credentials_t* loadWiFiCredentials() {
    if (credentials) {
        return credentials;
    }
    nvs_handle_t storage;
    size_t value_size;
    nvs_open(NVS_PREFERENCES_STORAGE_NAME, NVS_READWRITE, &storage);
    nvs_get_str(storage, NVS_WIFI_SSID_CREDENTIALS_KEY, NULL, &value_size);
    if (value_size && value_size < 512) {
        credentials = new wifi_credentials_t;
        char buffer[value_size] = {0};
        nvs_get_str(storage, NVS_WIFI_SSID_CREDENTIALS_KEY, buffer, &value_size);
        credentials->ssid = buffer;
        nvs_get_str(storage, NVS_WIFI_PASSWORD_CREDENTIALS_KEY, NULL, &value_size);
        if (value_size && value_size < 512) {
            char buffer[value_size] = {0};
            nvs_get_str(storage, NVS_WIFI_PASSWORD_CREDENTIALS_KEY, buffer, &value_size);
            credentials->password = buffer;
        }
    }
    nvs_close(storage);

    #ifdef DEFAULT_WIFI_SSID_1
    appendToCredentials(DEFAULT_WIFI_SSID_1, DEFAULT_WIFI_PASSWORD_1);
    #endif
    #ifdef DEFAULT_WIFI_SSID_2
    appendToCredentials(DEFAULT_WIFI_SSID_2, DEFAULT_WIFI_PASSWORD_2);
    #endif
    #ifdef DEFAULT_WIFI_SSID_3
    appendToCredentials(DEFAULT_WIFI_SSID_3, DEFAULT_WIFI_PASSWORD_3);
    #endif

    return credentials;
}

void saveWiFiCredentials(const char* ssid, const char* password) {
    nvs_handle_t storage;
    nvs_open(NVS_PREFERENCES_STORAGE_NAME, NVS_READWRITE, &storage);
    nvs_set_str(storage, NVS_WIFI_SSID_CREDENTIALS_KEY, ssid);
    nvs_set_str(storage, NVS_WIFI_PASSWORD_CREDENTIALS_KEY, password);
    nvs_close(storage);
}

void cleanWiFiCredentials() {
    while(credentials) {
        wifi_credentials_t* current = credentials;
        credentials = credentials->next;
        delete current;
    }
}

void saveSettings(preferences_t* prefs) {
    nvs_handle_t storage;
    nvs_open(NVS_PREFERENCES_STORAGE_NAME, NVS_READWRITE, &storage);
    // if (prefs->wifi_ssid.size() > 0) {
    //     nvs_set_str(storage, NVS_WIFI_SSID_CREDENTIALS_KEY, prefs->wifi_ssid.c_str());
    // }
    // if (prefs->wifi_password.size() > 0) {
    //     nvs_set_str(storage, NVS_WIFI_PASSWORD_CREDENTIALS_KEY, prefs->wifi_password.c_str());
    // }
    if (prefs->high_pollution_value) {
        nvs_set_u16(storage, NVS_HIGH_POLLUTION_VALUE_KEY, prefs->high_pollution_value);
    }
    if (prefs->min_thermometer_temperature) {
        nvs_set_i8(storage, NVS_MIN_THERMOMETER_TEMPERATURE_KEY, prefs->min_thermometer_temperature);
    }
    if (prefs->max_thermometer_temperature) {
        nvs_set_i8(storage, NVS_MAX_THERMOMETER_TEMPERATURE_KEY, prefs->max_thermometer_temperature);
    }
    if (prefs->measurement_period) {
        nvs_set_u16(storage, NVS_MEASUREMENT_PERIOD_KEY, prefs->measurement_period);
    }
    if (prefs->time_sync_period) {
        nvs_set_u16(storage, NVS_TIME_SYNC_PERIOD_KEY, prefs->time_sync_period);
    }
    if (prefs->history_length) {
        nvs_set_u16(storage, NVS_HISTORY_LENGTH_KEY, prefs->history_length);
    }
    if (prefs->history_record_period) {
        nvs_set_u16(storage, NVS_HISTORY_RECORD_PERIOD_KEY, prefs->history_record_period);
    }
    delete prefs;
    nvs_close(storage);
}

void setupPreferences() {
    preferences = new preferences_t;
    nvs_handle_t storage;
    nvs_open(NVS_PREFERENCES_STORAGE_NAME, NVS_READWRITE, &storage);
    nvs_get_u16(storage, NVS_HIGH_POLLUTION_VALUE_KEY, &(preferences->high_pollution_value));
    if (!preferences->high_pollution_value) {
        preferences->high_pollution_value = NVS_HIGH_POLLUTION_VALUE_DEFAULT;
    }
    nvs_get_i8(storage, NVS_MIN_THERMOMETER_TEMPERATURE_KEY, &(preferences->min_thermometer_temperature));
    if (!preferences->min_thermometer_temperature) {
        preferences->min_thermometer_temperature = NVS_MIN_THERMOMETER_TEMPERATURE_DEFAULT;
    }
    nvs_get_i8(storage, NVS_MAX_THERMOMETER_TEMPERATURE_KEY, &(preferences->max_thermometer_temperature));
    if (!preferences->max_thermometer_temperature) {
        preferences->max_thermometer_temperature = NVS_MAX_THERMOMETER_TEMPERATURE_DEFAULT;
    }
    nvs_get_u16(storage, NVS_MEASUREMENT_PERIOD_KEY, &(preferences->measurement_period));
    if (!preferences->measurement_period) {
        preferences->measurement_period = NVS_MEASUREMENT_PERIOD_DEFAULT;
    }
    nvs_get_u16(storage, NVS_TIME_SYNC_PERIOD_KEY, &(preferences->time_sync_period));
    if (!preferences->time_sync_period) {
        preferences->time_sync_period = NVS_TIME_SYNC_PERIOD_DEFAULT;
    }
    nvs_get_u16(storage, NVS_HISTORY_LENGTH_KEY, &(preferences->history_length));
    if (!preferences->history_length) {
        preferences->history_length = NVS_HISTORY_LENGTH_DEFAULT;
    }
    nvs_get_u16(storage, NVS_HISTORY_RECORD_PERIOD_KEY, &(preferences->history_record_period));
    if (!preferences->history_record_period) {
        preferences->history_record_period = NVS_HISTORY_RECORD_PERIOD_DEFAULT;
    }
    nvs_close(storage);
}

preferences_t* getPreferences() {
    return preferences;
}