#include "display.h"

TFT_eSPI tft = TFT_eSPI();
bool thermometerInitialized = false;
bool pressureIconInitialized = false;
bool pollutionWarningInitialized = false;

void drawTemperatureUnits() {
    tft.fillCircle(TEMPERATURE_POSITION_X - 11, TEMPERATURE_POSITION_Y - 1, 3, FOREGROUND_COLOR);
    tft.fillCircle(TEMPERATURE_POSITION_X - 11, TEMPERATURE_POSITION_Y - 1, 2, BACKGROUND_COLOR);
    tft.drawRightString("C", TEMPERATURE_POSITION_X, TEMPERATURE_POSITION_Y, 2);
}

void cleanTemperature() {
    tft.fillRect(TEMPERATURE_POSITION_X - 102, TEMPERATURE_POSITION_Y - 15, TEMPERATURE_WIDTH, TEMPERATURE_HEIGHT, BACKGROUND_COLOR);
}

void drawTemperature(float value) {
    cleanTemperature();
    int8_t integerPart = (int8_t)value;
    uint8_t decimalPart = (uint8_t)round((abs(value) - abs(integerPart)) * 10);
    char buff[5] = {0};

    snprintf(buff, 4, "%d.", integerPart);
    tft.drawRightString(buff, TEMPERATURE_POSITION_X - 30, TEMPERATURE_POSITION_Y - 13, 6);

    snprintf(buff, 2, "%d", decimalPart);
    tft.drawRightString(buff, TEMPERATURE_POSITION_X - 16, TEMPERATURE_POSITION_Y + 4, 4);
}

void cleanTime() {
    tft.fillRect(CLOCK_POSITION_X - CLOCK_WIDTH + 2, CLOCK_POSITION_Y - 1, CLOCK_WIDTH, CLOCK_HEIGHT, BACKGROUND_COLOR);
}

void drawTime(time_t* now) {
    if (pollutionWarningInitialized) {
        return;
    }
    cleanTime();
    char buff[6] = {0};
    struct tm *tmp = gmtime(now);
    snprintf(buff, 6, "%02d:%02d", tmp->tm_hour + TIMEZONE_OFFSET, tmp->tm_min);

    tft.setTextColor(CLOCK_COLOR, BACKGROUND_COLOR);
    tft.drawRightString(buff, 123, 91, 4);
    tft.setTextColor(FOREGROUND_COLOR, BACKGROUND_COLOR);
}

void drawTime() {
    time_t now;
    time(&now);
    drawTime(&now);
}

void cleanPollutionWarning() {
    tft.fillRect(WARNING_POSITION_X - 30, WARNING_POSITION_Y - 20, WARNING_WIDTH, WARNING_HEIGHT, BACKGROUND_COLOR);
}

void drawPollutionWarning() {
    cleanPollutionWarning();
    tft.fillTriangle(WARNING_POSITION_X, WARNING_POSITION_Y - 18, WARNING_POSITION_X + 28, WARNING_POSITION_Y + 28, WARNING_POSITION_X - 28, WARNING_POSITION_Y + 28, FOREGROUND_COLOR);
    tft.fillTriangle(WARNING_POSITION_X, WARNING_POSITION_Y - 14, WARNING_POSITION_X + 24, WARNING_POSITION_Y + 26, WARNING_POSITION_X - 24, WARNING_POSITION_Y + 26, COLOR_RED);
    tft.setTextColor(FOREGROUND_COLOR, COLOR_RED);
    tft.drawCentreString("!", WARNING_POSITION_X, WARNING_POSITION_Y, 4);
    tft.setTextColor(FOREGROUND_COLOR, BACKGROUND_COLOR);
    tft.drawRightString("C O", WARNING_POSITION_X + 65, WARNING_POSITION_Y - 7, 4);
}

void drawPollutionUnits() {
    tft.drawRightString("M", POLLUTION_POSITION_X - 7, POLLUTION_POSITION_Y + 1, 1);
    tft.drawLine(POLLUTION_POSITION_X - 6, POLLUTION_POSITION_Y + 1, POLLUTION_POSITION_X - 3, POLLUTION_POSITION_Y + 1, POLLUTION_COLOR);
    tft.drawLine(POLLUTION_POSITION_X - 6, POLLUTION_POSITION_Y + 1, POLLUTION_POSITION_X - 6, POLLUTION_POSITION_Y + 7, POLLUTION_COLOR);

    tft.drawLine(POLLUTION_POSITION_X - 15, POLLUTION_POSITION_Y + 9, POLLUTION_POSITION_X - 1, POLLUTION_POSITION_Y + 9, POLLUTION_COLOR);

    tft.drawRightString("M", POLLUTION_POSITION_X - 7, POLLUTION_POSITION_Y + 12, 1);
    tft.drawRightString("3", POLLUTION_POSITION_X, POLLUTION_POSITION_Y + 11, 1);
}

void cleanPollution() {
    tft.fillRect(POLLUTION_POSITION_X - 99, POLLUTION_POSITION_Y, POLLUTION_WIDTH, POLLUTION_HEIGHT, BACKGROUND_COLOR);
}

void drawPollution(float value) {
    cleanPollution();
    if (value >= 0 && value < 10000) {
        uint16_t integerPart = (uint16_t)value;
        uint8_t decimalPart = (uint8_t)round((value - integerPart) * 100);
        char buff[6] = {0};

        snprintf(buff, 3, "%d0", decimalPart);
        tft.drawRightString(buff, POLLUTION_POSITION_X - 17, POLLUTION_POSITION_Y + 6, 2);

        snprintf(buff, 6, "%d.", integerPart);
        tft.drawRightString(buff, POLLUTION_POSITION_X - 34, POLLUTION_POSITION_Y, 4);
    } else {
        char buff[7] = {0};
        snprintf(buff, 7, "%.0f", value);
        tft.drawRightString(buff, POLLUTION_POSITION_X - 18, POLLUTION_POSITION_Y, 4);
    }

    if (value >= getPreferences()->high_pollution_value) {
        if (!pollutionWarningInitialized) {
            drawPollutionWarning();
            pollutionWarningInitialized = true;
        }
    } else {
        if (pollutionWarningInitialized) {
            cleanPollutionWarning();
            pollutionWarningInitialized = false;
            drawTime();
        }
    }
}

void drawPressureIcon() {
    tft.fillTriangle(PRESSURE_POSITION_X - 98, PRESSURE_POSITION_Y + 12, PRESSURE_POSITION_X - 95, PRESSURE_POSITION_Y + 16, PRESSURE_POSITION_X - 92, PRESSURE_POSITION_Y + 12, PRESSURE_COLOR);
    tft.fillRect(PRESSURE_POSITION_X - 96, PRESSURE_POSITION_Y + 5, 3, 7, PRESSURE_COLOR);

    tft.fillTriangle(PRESSURE_POSITION_X - 92, PRESSURE_POSITION_Y + 11, PRESSURE_POSITION_X - 89, PRESSURE_POSITION_Y + 15, PRESSURE_POSITION_X - 86, PRESSURE_POSITION_Y + 11, PRESSURE_COLOR);
    tft.fillRect(PRESSURE_POSITION_X - 90, PRESSURE_POSITION_Y + 6, 3, 6, PRESSURE_COLOR);
}

void drawPressureUnits() {
    tft.setTextColor(PRESSURE_COLOR, BACKGROUND_COLOR);
    tft.drawRightString("MM", PRESSURE_POSITION_X, PRESSURE_POSITION_Y - 2, 1);
    tft.drawRightString("PT", PRESSURE_POSITION_X, PRESSURE_POSITION_Y + 6, 1);
    tft.drawRightString("CT", PRESSURE_POSITION_X, PRESSURE_POSITION_Y + 14, 1);
    tft.setTextColor(FOREGROUND_COLOR, BACKGROUND_COLOR);
}

void cleanPressure(float value) {
    if (value >= 1000 || value < -100) {
        tft.fillRect(PRESSURE_POSITION_X - 101, PRESSURE_POSITION_Y, PRESSURE_WIDTH, PRESSURE_HEIGHT, BACKGROUND_COLOR);
        pressureIconInitialized = false;
    } else {
        if (!pressureIconInitialized) {
            tft.fillRect(PRESSURE_POSITION_X - 101, PRESSURE_POSITION_Y, PRESSURE_WIDTH, PRESSURE_HEIGHT, BACKGROUND_COLOR);
            drawPressureIcon();
            pressureIconInitialized = true;
        } else {
            tft.fillRect(PRESSURE_POSITION_X - 85, PRESSURE_POSITION_Y, PRESSURE_WIDTH - 16, PRESSURE_HEIGHT, BACKGROUND_COLOR);
        }
    }
}

void drawPressure(float value) {
    cleanPressure(value);
    tft.setTextColor(PRESSURE_COLOR, BACKGROUND_COLOR);
    if (value > 0 && value < 10000) {
        uint16_t integerPart = (uint16_t)value;
        uint8_t decimalPart = (uint8_t)round((value - integerPart) * 100);
        char buff[5] = {0};

        snprintf(buff, 3, "%d0", decimalPart);
        tft.drawRightString(buff, PRESSURE_POSITION_X - 14, PRESSURE_POSITION_Y + 6, 2);

        snprintf(buff, 6, "%d.", integerPart);
        tft.drawRightString(buff, PRESSURE_POSITION_X - 34, PRESSURE_POSITION_Y, 4);
    } else {
        char buff[8] = {0};
        snprintf(buff, 8, "%.0f", value);
        tft.drawRightString(buff, PRESSURE_POSITION_X - 14, PRESSURE_POSITION_Y, 4);
    }
    tft.setTextColor(FOREGROUND_COLOR, BACKGROUND_COLOR);
}

void initThermometer() {
    tft.fillSmoothRoundRect(THERMOMETER_X - THERMOMETER_BORDER, THERMOMETER_Y - THERMOMETER_BORDER, THERMOMETER_WIDTH + 2 * THERMOMETER_BORDER, THERMOMETER_MAX_LENGTH + 2 * THERMOMETER_BORDER, round(THERMOMETER_WIDTH + 2 * THERMOMETER_BORDER) + 1, FOREGROUND_COLOR, FOREGROUND_COLOR);
    tft.fillSmoothCircle(13, 144, THERMOMETER_RADIUS + THERMOMETER_BORDER, FOREGROUND_COLOR, FOREGROUND_COLOR);
    tft.fillSmoothCircle(13, 144, THERMOMETER_RADIUS, COLOR_RED, COLOR_RED);
    tft.fillSmoothRoundRect(THERMOMETER_X, THERMOMETER_Y, THERMOMETER_WIDTH, THERMOMETER_MAX_LENGTH, THERMOMETER_WIDTH, BACKGROUND_COLOR, BACKGROUND_COLOR);
}

void drawThermometer(float value) {
    if (!thermometerInitialized) {
        initThermometer();
        drawPollutionUnits();
        drawTemperatureUnits();
        drawPressureUnits();
        thermometerInitialized = true;
    }
    uint16_t mercuryValue;
    preferences_t* preferences = getPreferences();
    float thermometerStep = (THERMOMETER_MAX_LENGTH / (preferences->max_thermometer_temperature - preferences->min_thermometer_temperature));
    if (value <= preferences->min_thermometer_temperature) {
        mercuryValue = thermometerStep;
    } else if (value >= preferences->max_thermometer_temperature) {
        mercuryValue = THERMOMETER_MAX_LENGTH;
    } else {
        mercuryValue = round((value - preferences->min_thermometer_temperature) * thermometerStep);
    }
    tft.fillSmoothRoundRect(THERMOMETER_X, THERMOMETER_Y, THERMOMETER_WIDTH, THERMOMETER_MAX_LENGTH, THERMOMETER_WIDTH, BACKGROUND_COLOR, BACKGROUND_COLOR);
    tft.fillSmoothRoundRect(THERMOMETER_X, THERMOMETER_Y + THERMOMETER_MAX_LENGTH - mercuryValue, THERMOMETER_WIDTH, mercuryValue, THERMOMETER_WIDTH, COLOR_RED, COLOR_RED);

    for (uint8_t idx = 0; idx < preferences->max_thermometer_temperature - preferences->min_thermometer_temperature - 1; idx++) {
        if (!idx) {
            continue;
        }
        tft.fillRect(THERMOMETER_SCALE_UNIT_X, round(THERMOMETER_Y + idx * thermometerStep), THERMOMETER_SCALE_UNIT_LENGTH, THERMOMETER_SCALE_UNIT_WIDTH, FOREGROUND_COLOR);
    }
}

bool setupDisplay() {
    tft.init();
    tft.fillScreen(BACKGROUND_COLOR);
    tft.setTextColor(FOREGROUND_COLOR, BACKGROUND_COLOR);
    return true;
}