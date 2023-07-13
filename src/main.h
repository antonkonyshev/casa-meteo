#define CONFIG_ESP_INT_WDT_TIMEOUT_MS 5000    // milliseconds

#include <Arduino.h>
#include <time.h>

#define MEASUREMENT_PERIOD 5  // seconds
#define TIME_SYNC_PERIODICITY 120  // every N periodical measurements (to don't use separate MCU timer for this)
