#define CONFIG_ESP_INT_WDT_TIMEOUT_MS 5000    // milliseconds

#include "pinout.h"

#ifdef ESP8266_DEVICE
#include <Esp.h>
#endif

#include "CasaLib/logging.h"
#include "CasaLib/network.h"
#include "CasaLib/ntp.h"

#include "server.h"