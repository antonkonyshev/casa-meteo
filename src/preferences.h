#include <string>
#include <nvs_flash.h>

#define NVS_WIFI_SSID_CREDENTIALS_KEY "wifi_ssid"
#define NVS_WIFI_PASSWORD_CREDENTIALS_KEY "wifi_password"
#define NVS_CREDENTIALS_STORAGE_NAME "credentials"

typedef struct wifi_credentials_s {
    std::string ssid;
    std::string password;
    wifi_credentials_s* next;

    wifi_credentials_s() : next(nullptr) {}
} wifi_credentials_t;

wifi_credentials_t* loadWiFiCredentials();
void saveWiFiCredentials(const char* ssid, const char* password);
void cleanWiFiCredentials();