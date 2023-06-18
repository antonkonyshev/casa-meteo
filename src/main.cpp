#define CONFIG_ESP_INT_WDT_TIMEOUT_MS 5000  // milliseconds

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>
#include <Adafruit_BMP280.h>

#define ARDUINOJSON_USE_DOUBLE 0
#include <ArduinoJson.h>

#define LED_PIN 2
#define BMP280_SDA 21
#define BMP280_SCL 22
#define BMP280_I2C_ADDRESS 0x76
#define MMHG_IN_PA 0.007500616827041699
#define API_PORT 80

// In order to have last 24 hours use 5 minutes period and
// history length of 288 (user requests aren't considered),
// it'll consume 9216 bytes of memory, may be increased
// significantly in case more details are needed.
#define MEASUREMENT_PERIOD 60  // seconds; in order to have last 24 hours, use 5m period and history length of 288, it'll require 
#define HISTORY_LENGTH 60  // measurements

static const char* wifi_ssid = "AKTpLAp";
static const char* wifi_password = "JXfRV9QRD7EaFW6UAPNPoG2BdXB2LcpETSsJmpmX7R9gr5Xn6qoyq2bpLKCn7A4";

Adafruit_BMP280 bmp280;
WebServer server(API_PORT);
StaticJsonDocument<256> jsonDocument;  // ~200 length de facto
char buffer[256];
time_t now = 0;
hw_timer_t* measurement_timer = NULL;
bool perform_periodical_measurement = false;
uint8_t history_index = 0;

typedef struct measurement_s {
  uint32_t timestamp;
  float temperature;
  float pressure;
  float altitude;
} measurement_t;

measurement_t* history[HISTORY_LENGTH];

void loadTime() {
  time(&now);
}

measurement_t* loadSensorData() {
  measurement_t* measurement;
  measurement = (measurement_t*) malloc(sizeof(measurement_t));
  measurement->timestamp = now;
  Serial.print("Timestamp: ");
  Serial.print(measurement->timestamp);
  measurement->temperature = round(bmp280.readTemperature() * 100) / 100;
  Serial.print("   |   Temperature: ");
  Serial.print(measurement->temperature);
  Serial.print(" °C   |   Pressure: ");
  measurement->pressure = round(bmp280.readPressure() * MMHG_IN_PA * 100) / 100;
  Serial.print(measurement->pressure);
  Serial.print(" mmHg   |   Altitude: ");
  measurement->altitude = round(bmp280.readAltitude() * 100) / 100;
  Serial.print(measurement->altitude);
  Serial.println(" m");
  if (history_index < HISTORY_LENGTH - 1) {
    history_index += 1;
  } else {
    history_index = 0;
  }
  if (history[history_index] != NULL) {
    measurement_t* old_measurement = history[history_index];
    free(old_measurement);
  }
  history[history_index] = measurement;
  return measurement;
}

void createJson(measurement_t* measurement) {
  jsonDocument.clear();
  jsonDocument["timestamp"] = measurement->timestamp;
  JsonObject obj = jsonDocument.createNestedObject("temperature");
  obj["value"] = measurement->temperature;
  obj["unit"] = "°C";
  obj = jsonDocument.createNestedObject("pressure");
  obj["value"] = measurement->pressure;
  obj["unit"] = "mmHg";
  obj = jsonDocument.createNestedObject("altitude");
  obj["value"] = measurement->altitude;
  obj["unit"] = "m";
  serializeJson(jsonDocument, buffer);
}

void performPeriodicalMeasurement() {
  perform_periodical_measurement = true;
}

void getMeteoResponse() {
  digitalWrite(LED_PIN, HIGH);
  createJson(loadSensorData());
  server.send(200, "application/json", buffer);
  digitalWrite(LED_PIN, LOW);
}

void getServiceID() {
  server.send(200, "plain/text", "meteo");
}

void setupRouting() {
  server.on("/", getMeteoResponse);
  server.on("/service", getServiceID);
  server.begin();
}

void setup() {
  Serial.begin(9600);
  Serial.println("");
  Serial.println("--------------------------------- Meteo ---------------------------------");
  delay(2000);

  Serial.print("Initializing LED indication ...");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  Serial.println(" [ Done ]");
  Serial.print("Initializing BMP280 ...");
  if (bmp280.begin(BMP280_I2C_ADDRESS)) {
    Serial.println(" [ Done ]");
  } else {
    Serial.println(" [ Fail ]");
  }

  Serial.print("Connecting to WiFi network ...");
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println(" [ Done ]");

  Serial.print("Starting HTTP API server ...");
  setupRouting();
  Serial.println(" [ Done ]");

  Serial.print("Setting up the real time clock ...");
  configTime(0, 0, "pool.ntp.org", "time.google.com", "time.cloudflare.com");
  while (now < 1687100000) {
    loadTime();
    Serial.print(".");
    delay(100);
  }
  Serial.println(" [ Done ]");

  Serial.print("Setting up timers ...");
  measurement_timer = timerBegin(0, 8000, true);
  timerAttachInterrupt(measurement_timer, &performPeriodicalMeasurement, true);
  timerAlarmWrite(measurement_timer, MEASUREMENT_PERIOD * 10000, true);
  timerAlarmEnable(measurement_timer);
  Serial.println(" [ Done ]");

  Serial.print("Local IP address: ");
  Serial.println(WiFi.localIP());

  Serial.printf("Total  heap: %8d bytes   |   Free  heap: %8d bytes\n", ESP.getHeapSize(), ESP.getFreeHeap());
  Serial.printf("Total PSRAM: %8d bytes   |   Free PSRAM: %8d bytes\n", ESP.getPsramSize(), ESP.getFreePsram());
  Serial.printf("Sketch size: %8d bytes   |   Free space: %8d bytes\n", ESP.getSketchSize(), ESP.getFreeSketchSpace());

  Serial.println("");
  loadSensorData();
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  server.handleClient();
  if (perform_periodical_measurement) {
    digitalWrite(LED_PIN, HIGH);
    loadSensorData();
    perform_periodical_measurement = false;
    digitalWrite(LED_PIN, LOW);
  }
}