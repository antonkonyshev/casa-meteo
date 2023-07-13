#include <TFT_eSPI.h>
#include <SPI.h>

#include "pinout.h"

// TODO: make HIGH_POLLUTION_VALUE an editable setting controlled from the application.
#define HIGH_POLLUTION_VALUE 20  // mgm3, warns a user in case if the current pollution value is higher

#define COLOR_BLUE TFT_RED
#define COLOR_RED TFT_BLUE
#define COLOR_WHITE TFT_WHITE
#define COLOR_BLACK TFT_BLACK
#define COLOR_LIGHTGREEN TFT_GREEN
#define COLOR_GREEN TFT_DARKGREEN
#define COLOR_YELLOW TFT_CYAN
#define COLOR_OLIVE TFT_DARKCYAN
#define COLOR_DARKBLUE TFT_OLIVE
#define COLOR_DARKGRAY TFT_DARKGREY
#define COLOR_LIGHTGRAY TFT_LIGHTGREY
#define COLOR_SILVER TFT_SILVER
#define COLOR_SANDY TFT_SKYBLUE

#define FOREGROUND_COLOR COLOR_WHITE
#define BACKGROUND_COLOR COLOR_BLACK

// TODO: allow setting of max and min temperature as preferences
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

#define TEMPERATURE_POSITION_X 126
#define TEMPERATURE_POSITION_Y 18
#define TEMPERATURE_WIDTH 88
#define TEMPERATURE_HEIGHT 40

#define PRESSURE_POSITION_X 126
#define PRESSURE_POSITION_Y 51
#define PRESSURE_WIDTH 87
#define PRESSURE_HEIGHT 20
#define PRESSURE_COLOR COLOR_SANDY

#define TIMEZONE_OFFSET 3
#define CLOCK_POSITION_X 123
#define CLOCK_POSITION_Y 91
#define CLOCK_WIDTH 67
#define CLOCK_HEIGHT 22
#define CLOCK_COLOR COLOR_SANDY

#define POLLUTION_POSITION_X 125
#define POLLUTION_POSITION_Y 135
#define POLLUTION_WIDTH 82
#define POLLUTION_HEIGHT 21
#define POLLUTION_COLOR COLOR_WHITE

#define WARNING_POSITION_X 58
#define WARNING_POSITION_Y 98
#define WARNING_WIDTH 97
#define WARNING_HEIGHT 52

void drawThermometer(float);
void drawTemperature(float);
void drawPollution(float);
void drawPressure(float);
void drawTime();
void drawTime(time_t*);
bool setupDisplay();