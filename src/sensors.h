#include <Adafruit_BMP280.h>

#include "pinout.h"
#include "display.h"

#define BMP280_I2C_ADDRESS 0x76
#define MMHG_IN_PA 0.007500616827041699
#define SERIALIZED_MEASUREMENT_MAX_LENGTH 512    // near 245 bytes per measurement

#define MQ7_VOLTAGE 3.3

float readTemperature();
float readAltitude();
float readPressure();
float convertPollutionToMgM3(int);
float readPollution();
bool setupBmp280();
bool setupMq7();
measurement_t* loadSensorData();
char* getLastMeasurementSerialized();