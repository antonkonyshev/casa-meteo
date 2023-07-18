#include "sensors.h"

Adafruit_BMP280 bmp280;
char lastMeasurementSerialized[SERIALIZED_MEASUREMENT_MAX_LENGTH] = {0};

char* getLastMeasurementSerialized() {
    return lastMeasurementSerialized;
}

float readTemperature() {
    return round(bmp280.readTemperature() * 100) / 100;
}

float readAltitude() {
    return round(bmp280.readAltitude() * 100) / 100;
}

float readPressure() {
    return round(bmp280.readPressure() * MMHG_IN_PA * 100) / 100;
}

float convertPollutionToMgM3(int value) {
    return (3.027 * exp(1.0698 * (value * (MQ7_VOLTAGE / 4095)))) * (28.06 / 24.45);
}

float readPollution() {
    return round(convertPollutionToMgM3(analogRead(MQ7_AO)) * 100) / 100;
}

bool setupBmp280() {
    return bmp280.begin(BMP280_I2C_ADDRESS);
}

bool setupMq7() {
    pinMode(MQ7_AO, INPUT);
    return true;
}

measurement_t* loadSensorData() {
    time_t now;
    measurement_t* measurement = new measurement_t;

    time(&now);
    measurement->timestamp = now;
    drawTime(&now);

    measurement->temperature = readTemperature();
    drawTemperature(measurement->temperature);
    drawThermometer(measurement->temperature);

    measurement->pressure = readPressure();
    drawPressure(measurement->pressure);

    measurement->altitude = readAltitude();

    measurement->pollution = readPollution();
    drawPollution(measurement->pollution);

    // This allows us to rid of excess dependency (json serialization) and 
    // reduce client waiting time, when the client comes to get measurements
    // everything we need is to send him already prepared char array
    snprintf(lastMeasurementSerialized, SERIALIZED_MEASUREMENT_MAX_LENGTH,
        "{\"timestamp\":%d,\"temperature\":%.2f,\"pressure\":%.2f,\"altitude\":%.2f,\"pollution\":%.2f}",
        measurement->timestamp, measurement->temperature, measurement->pressure, measurement->altitude,
        measurement->pollution);

    periodicalAppendToHistory(now, std::string(lastMeasurementSerialized));

    return measurement;
}
