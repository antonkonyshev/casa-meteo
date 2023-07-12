#include <cstdlib>

#define HISTORY_LENGTH 50    // 300    // measurements

typedef struct measurement_s {
    uint32_t timestamp;    // UTC timezone
    float temperature;    // °C
    float pressure;    // mmHg
    float altitude;    // meters above sea level
    float pollution;    // mg/m3
} measurement_t;

typedef struct history_record_s {
    measurement_t* measurement;
    history_record_s* next;
    history_record_s* prev;
} history_record_t;

typedef struct history_s {
    history_record_t* first;
    history_record_t* last;
    unsigned int length;
} history_t;

history_record_t* appendToHistory(measurement_t*);
history_t* getHistory();
history_t* setupHistory();