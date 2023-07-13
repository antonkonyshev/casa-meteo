#include <cstdlib>

#define HISTORY_LENGTH 50  // number of measurements stored in history
#define HISTORY_RECORDS_PERIOD 1800  // seconds

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
history_record_t* periodicalAppendToHistory(measurement_t*);
history_t* getHistory();
history_t* setupHistory();