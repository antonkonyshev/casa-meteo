#pragma once

#include <ESPAsyncWebServer.h>

#include "pinout.h"
#include "preferences.h"
#include "sensors.h"

#define API_PORT 80

void setupRouting();