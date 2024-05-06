#pragma once

#include <ESPAsyncWebServer.h>

#include "pinout.h"
#include "CasaLib/server.h"
#include "sensors.h"

#define API_PORT 80

void serviceInfoResponsePayload(char* buffer);
void setupRouting();