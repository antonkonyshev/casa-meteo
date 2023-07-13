#include "preferences.h"

wifi_credentials_t* credentials;

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
    nvs_open(NVS_CREDENTIALS_STORAGE_NAME, NVS_READWRITE, &storage);
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
    nvs_open(NVS_CREDENTIALS_STORAGE_NAME, NVS_READWRITE, &storage);
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