#include <Adafruit_BMP280.h>

#include "pinout.h"
#include "measurement.h"
#include "display.h"

#define BMP280_I2C_ADDRESS 0x76
#define MMHG_IN_PA 0.007500616827041699

#define MQ7_VOLTAGE 3.3

float readTemperature();
float readAltitude();
float readPressure();
float convertPollutionToMgM3(int);
float readPollution();
bool setupBmp280();
bool setupMq7();
measurement_t* loadSensorData();