#include <display.h>

TFT_eSPI tft = TFT_eSPI();
bool thermometerInitialized = false;
bool pressureIconInitialized = false;
bool pollutionWarningInitialized = false;

void initThermometer() {
    tft.fillSmoothRoundRect(THERMOMETER_X - THERMOMETER_BORDER, THERMOMETER_Y - THERMOMETER_BORDER, THERMOMETER_WIDTH + 2 * THERMOMETER_BORDER, THERMOMETER_MAX_LENGTH + 2 * THERMOMETER_BORDER, round(THERMOMETER_WIDTH + 2 * THERMOMETER_BORDER) + 1, COLOR_WHITE, COLOR_WHITE);
    tft.fillSmoothCircle(13, 144, THERMOMETER_RADIUS + THERMOMETER_BORDER, COLOR_WHITE, COLOR_WHITE);
    tft.fillSmoothCircle(13, 144, THERMOMETER_RADIUS, COLOR_RED, COLOR_RED);
    tft.fillSmoothRoundRect(THERMOMETER_X, THERMOMETER_Y, THERMOMETER_WIDTH, THERMOMETER_MAX_LENGTH, THERMOMETER_WIDTH, COLOR_BLACK, COLOR_BLACK);
}

void drawPollutionUnits() {
    tft.drawRightString("M", 118, 136, 1);
    tft.drawLine(119, 136, 122, 136, COLOR_WHITE);
    tft.drawLine(119, 136, 119, 142, COLOR_WHITE);

    tft.drawLine(110, 144, 124, 144, COLOR_WHITE);

    tft.drawRightString("M", 118, 147, 1);
    tft.drawRightString("3", 125, 146, 1);
}

void drawTemperatureUnits() {
    tft.fillCircle(115, 17, 3, COLOR_WHITE);
    tft.fillCircle(115, 17, 2, COLOR_BLACK);
    tft.drawRightString("C", 126, 18, 2);
}

void drawPressureUnits() {
    tft.setTextColor(COLOR_SANDY, COLOR_BLACK);
    tft.drawRightString("MM", 126, 49, 1);
    tft.drawRightString("PT", 126, 57, 1);
    tft.drawRightString("CT", 126, 65, 1);
    tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
}

void drawPressureIcon() {
    tft.fillTriangle(28, 63, 31, 67, 34, 63, COLOR_SANDY);
    tft.fillRect(30, 56, 3, 7, COLOR_SANDY);

    tft.fillTriangle(34, 62, 37, 66, 40, 62, COLOR_SANDY);
    tft.fillRect(36, 57, 3, 6, COLOR_SANDY);
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
    if (value <= THERMOMETER_MIN_TEMPERATURE) {
        mercuryValue = THERMOMETER_STEP;
    } else if (value >= THERMOMETER_MAX_TEMPERATURE) {
        mercuryValue = THERMOMETER_MAX_LENGTH;
    } else {
        mercuryValue = round((value - THERMOMETER_MIN_TEMPERATURE) * THERMOMETER_STEP);
    }
    tft.fillSmoothRoundRect(THERMOMETER_X, THERMOMETER_Y, THERMOMETER_WIDTH, THERMOMETER_MAX_LENGTH, THERMOMETER_WIDTH, COLOR_BLACK, COLOR_BLACK);
    tft.fillSmoothRoundRect(THERMOMETER_X, THERMOMETER_Y + THERMOMETER_MAX_LENGTH - mercuryValue, THERMOMETER_WIDTH, mercuryValue, THERMOMETER_WIDTH, COLOR_RED, COLOR_RED);

    for (uint8_t idx = 0; idx < THERMOMETER_MAX_TEMPERATURE - THERMOMETER_MIN_TEMPERATURE - 1; idx++) {
        if (!idx) {
            continue;
        }
        tft.fillRect(THERMOMETER_SCALE_UNIT_X, round(THERMOMETER_Y + idx * THERMOMETER_STEP), THERMOMETER_SCALE_UNIT_LENGTH, THERMOMETER_SCALE_UNIT_WIDTH, COLOR_WHITE);
    }
}

void cleanTemperature() {
    tft.fillRect(24, 3, 88, 40, COLOR_BLACK);
}

void drawTemperature(float value) {
    cleanTemperature();
    int8_t integerPart = (int8_t)value;
    uint8_t decimalPart = (uint8_t)round((abs(value) - abs(integerPart)) * 10);
    char buff[5] = {0};

    snprintf(buff, 4, "%d.", integerPart);
    tft.drawRightString(buff, 96, 5, 6);

    snprintf(buff, 2, "%d", decimalPart);
    tft.drawRightString(buff, 110, 22, 4);
}

void cleanTime() {
    tft.fillRect(58, 90, 67, 22, COLOR_BLACK);
}

void drawTime(time_t* now) {
    if (pollutionWarningInitialized) {
        return;
    }
    cleanTime();
    char buff[6] = {0};
    struct tm *tmp = gmtime(now);
    snprintf(buff, 6, "%02d:%02d", tmp->tm_hour + 3, tmp->tm_min);

    tft.setTextColor(COLOR_SANDY, COLOR_BLACK);
    tft.drawRightString(buff, 123, 91, 4);
    tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
}

void drawTime() {
    time_t now;
    time(&now);
    drawTime(&now);
}

void cleanPollutionWarning() {
    tft.fillRect(28, 78, 97, 52, COLOR_BLACK);
}

void drawPollutionWarning() {
    cleanPollutionWarning();
    tft.fillTriangle(58, 80, 86, 126, 30, 126, COLOR_WHITE);
    tft.fillTriangle(58, 84, 82, 124, 34, 124, COLOR_RED);
    tft.setTextColor(COLOR_WHITE, COLOR_RED);
    tft.drawCentreString("!", 58, 98, 4);
    tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
    tft.drawRightString("C O", 123, 91, 4);
}

void cleanPollution() {
    tft.fillRect(26, 135, 82, 21, COLOR_BLACK);
}

void drawPollution(float value) {
    cleanPollution();
    if (value >= 0 && value < 10000) {
        uint16_t integerPart = (uint16_t)value;
        uint8_t decimalPart = (uint8_t)round((value - integerPart) * 100);
        char buff[6] = {0};

        snprintf(buff, 3, "%d0", decimalPart);
        tft.drawRightString(buff, 108, 141, 2);

        snprintf(buff, 6, "%d.", integerPart);
        tft.drawRightString(buff, 91, 135, 4);
    } else {
        char buff[7] = {0};
        snprintf(buff, 7, "%.0f", value);
        tft.drawRightString(buff, 107, 135, 4);
    }

    if (value >= HIGH_POLLUTION_VALUE) {
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

void cleanPressure(float value) {
    if (value >= 1000 || value < -100) {
        tft.fillRect(25, 51, 87, 20, COLOR_BLACK);
        pressureIconInitialized = false;
    } else {
        if (!pressureIconInitialized) {
            tft.fillRect(25, 51, 87, 20, COLOR_BLACK);
            drawPressureIcon();
            pressureIconInitialized = true;
        } else {
            tft.fillRect(41, 51, 71, 20, COLOR_BLACK);
        }
    }
}

void drawPressure(float value) {
    cleanPressure(value);
    tft.setTextColor(COLOR_SANDY, COLOR_BLACK);
    if (value > 0 && value < 10000) {
        uint16_t integerPart = (uint16_t)value;
        uint8_t decimalPart = (uint8_t)round((value - integerPart) * 100);
        char buff[5] = {0};

        snprintf(buff, 3, "%d0", decimalPart);
        tft.drawRightString(buff, 112, 57, 2);

        snprintf(buff, 6, "%d.", integerPart);
        tft.drawRightString(buff, 92, 51, 4);
    } else {
        char buff[8] = {0};
        snprintf(buff, 8, "%.0f", value);
        tft.drawRightString(buff, 112, 51, 4);
    }
    tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
}

bool setupDisplay() {
    tft.init();
    tft.fillScreen(COLOR_BLACK);
    tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
    return true;
}