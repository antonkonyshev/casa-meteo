#include "sensors.h"

Adafruit_BMP280 bmp280;

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
    /*
    float mq7Voltage = 3.3;
    int coRaw = analogRead(MQ7_AO);    // Value from 0 to 4095
    Serial.print(coRaw);
    Serial.print(" [");
    double RvRo = coRaw * (MQ7_VOLTAGE / 4095);
    Serial.print(RvRo);
    Serial.print("|");
    int coPpm = 3.027 * exp(1.0698 * RvRo);
    Serial.print(coPpm);
    Serial.print(" ppm|");
    double mgm3 = coPpm * (28.06 / 24.45);
    Serial.print(mgm3);
    Serial.println(" mgm3]");
    */
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

    periodicalAppendToHistory(measurement);
    return measurement;
}
