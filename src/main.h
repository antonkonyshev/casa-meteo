#define CONFIG_ESP_INT_WDT_TIMEOUT_MS 5000    // milliseconds

#include <Arduino.h>
#include <time.h>

// In order to have last 24 hours use 5 minutes period and
// history length of 288 (user requests aren't considered),
// it'll consume 9216 bytes of memory, may be increased
// significantly in case more details are needed.
#define MEASUREMENT_PERIOD 5    // seconds
