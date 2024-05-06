#pragma once

#include "preferences.h"

typedef struct measurement_s {
    uint32_t timestamp;    // UTC timezone
    float temperature;    // °C
    float pressure;    // mmHg
    float altitude;    // meters above sea level
    float pollution;    // mg/m3
} measurement_t;

typedef struct history_record_s {
    const char* measurement;
    history_record_s* next;
    history_record_s* prev;

    history_record_s() : next(nullptr), prev(nullptr) {}
} history_record_t;

typedef struct history_s {
    history_record_t* first;
    history_record_t* last;
    unsigned int length;

    history_s() : first(nullptr), last(nullptr), length(0) {}
} history_t;

history_record_t* appendToHistory(const char* measurementSerialized);
history_record_t* periodicalAppendToHistory(time_t timestamp, const char* measurementSerialized);
history_t* getHistory();
history_t* setupHistory();