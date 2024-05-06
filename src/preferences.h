#pragma once

#include <ESPAsyncWebServer.h>

#include "CasaLib/preferences.h"

#define NVS_HIGH_POLLUTION_VALUE_KEY "high_pollution"
#define NVS_HIGH_POLLUTION_VALUE_DEFAULT 20
#define NVS_MAX_THERMOMETER_TEMPERATURE_KEY "max_temperature"
#define NVS_MAX_THERMOMETER_TEMPERATURE_DEFAULT 24
#define NVS_MIN_THERMOMETER_TEMPERATURE_KEY "min_temperature"
#define NVS_MIN_THERMOMETER_TEMPERATURE_DEFAULT 14
#define NVS_MEASUREMENT_PERIOD_KEY "measurement_period"
#define NVS_MEASUREMENT_PERIOD_DEFAULT 15
#define NVS_TIME_SYNC_PERIOD_KEY "time_sync_period"
#define NVS_TIME_SYNC_PERIOD_DEFAULT 1800
#define NVS_JOURNAL_LENGTH_KEY "journal_length"
#define NVS_JOURNAL_LENGTH_DEFAULT 40
#define NVS_HISTORY_RECORD_PERIOD_KEY "history_record_period"
#define NVS_HISTORY_RECORD_PERIOD_DEFAULT 1800

typedef struct preferences_s: base_preferences_t {
    uint16_t high_pollution_value;
    int8_t min_thermometer_temperature;
    int8_t max_thermometer_temperature;
    uint16_t measurement_period;
    uint16_t time_sync_period;
    uint16_t history_record_period;

    preferences_s() : high_pollution_value(NVS_HIGH_POLLUTION_VALUE_DEFAULT),
        min_thermometer_temperature(NVS_MIN_THERMOMETER_TEMPERATURE_DEFAULT),
        max_thermometer_temperature(NVS_MAX_THERMOMETER_TEMPERATURE_DEFAULT),
        measurement_period(NVS_MEASUREMENT_PERIOD_DEFAULT),
        time_sync_period(NVS_TIME_SYNC_PERIOD_DEFAULT),
        history_record_period(NVS_HISTORY_RECORD_PERIOD_DEFAULT) {}
} preferences_t;

void saveSettings(preferences_t* prefs);
void saveSettings(AsyncWebServerRequest* request);
void setupPreferences();
void serializeSettings(char* buffer);
preferences_t* getPreferences();