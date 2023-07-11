//
#include <TFT_eSPI.h>
#include <SPI.h>

#include "pinout.h"

#define THERMOMETER_MAX_TEMPERATURE 33  // thermometer max temperature in C, default: 22
#define THERMOMETER_MIN_TEMPERATURE 23  // thermometer min temperature in C, default: 12
#define THERMOMETER_MAX_LENGTH 142  // thermometer mercury length in pixels
#define THERMOMETER_BORDER 3   // border around mercury of the thermometer
#define THERMOMETER_WIDTH 9  // thermometer mercury width
#define THERMOMETER_X 9  // thermometer mercury x coordinate
#define THERMOMETER_Y 8  // thermometer mercury y coordinate
#define THERMOMETER_RADIUS 7  // radius of mercury ball of the thermometer in pixels
#define THERMOMETER_SCALE_UNIT_WIDTH 2  // in pixels
#define THERMOMETER_STEP (THERMOMETER_MAX_LENGTH / (THERMOMETER_MAX_TEMPERATURE - THERMOMETER_MIN_TEMPERATURE))  // per 1C
#define THERMOMETER_SCALE_UNIT_X round(THERMOMETER_X + THERMOMETER_WIDTH / 2) + 1
#define THERMOMETER_SCALE_UNIT_LENGTH round(THERMOMETER_WIDTH / 2)

#define COLOR_BLUE TFT_RED
#define COLOR_RED TFT_BLUE
#define COLOR_WHITE TFT_WHITE
#define COLOR_BLACK TFT_BLACK

void initThermometer();
void drawPollutionUnits();
void drawTemperatureUnits();
void drawThermometer(float);
void drawTemperature(float);
void drawPollution(float);
bool setupDisplay();