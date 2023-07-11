#include <Adafruit_BMP280.h>

#define HISTORY_LENGTH 50    // 300    // measurements

#define BMP280_I2C_ADDRESS 0x76
#define MMHG_IN_PA 0.007500616827041699

#define MQ7_VOLTAGE 3.3

typedef struct measurement_s {
    uint32_t timestamp;    // UTC timezone
    float temperature;    // °C
    float pressure;    // mmHg
    float altitude;    // meters above sea level
    float pollution;    // mg/m3
} measurement_t;

float readTemperature();
float readAltitude();
float readPressure();
float convertPollutionToMgM3(int);
float readPollution();
bool setupBmp280();
bool setupMq7();
void storeToHistory(measurement_t*);
measurement_t* loadSensorData();
measurement_t* getHistory();