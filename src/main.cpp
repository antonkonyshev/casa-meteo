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
#define MEASUREMENT_PERIOD 60  // seconds
#define HISTORY_LENGTH 50  // 300  // measurements
#define SERIALIZED_MEASUREMENT_MAX_LENGTH 256  // near 200 length de facto

static const char* wifi_ssid = "AKTpLAp";
static const char* wifi_password = "JXfRV9QRD7EaFW6UAPNPoG2BdXB2LcpETSsJmpmX7R9gr5Xn6qoyq2bpLKCn7A4";

Adafruit_BMP280 bmp280;
WebServer server(API_PORT);
time_t now = 0;
hw_timer_t* measurement_timer = NULL;
bool perform_periodical_measurement = false;
uint8_t history_index = -1;

typedef struct measurement_s {
  uint32_t timestamp;  // UTC timezone
  float temperature;  // °C
  float pressure;  // mmHg
  float altitude;  // meters above sea level
} measurement_t;

measurement_t* history[HISTORY_LENGTH];

measurement_t* loadSensorData() {
  measurement_t* measurement;
  measurement = (measurement_t*) malloc(sizeof(measurement_t));
  time(&now);
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

void createJson(measurement_t* measurement, JsonObject* json_object) {
  json_object->operator[]("timestamp") = measurement->timestamp;
  JsonObject obj = json_object->createNestedObject("temperature");
  obj["value"] = measurement->temperature;
  obj["unit"] = "°C";
  obj = json_object->createNestedObject("pressure");
  obj["value"] = measurement->pressure;
  obj["unit"] = "mmHg";
  obj = json_object->createNestedObject("altitude");
  obj["value"] = measurement->altitude;
  obj["unit"] = "m";
}

void performPeriodicalMeasurement() {
  perform_periodical_measurement = true;
}

void getServiceInfoResponse() {
  digitalWrite(LED_PIN, HIGH);
  StaticJsonDocument<128> json_document;
  char buffer[128] = { '\0' };
  json_document["service"] = "meteo";
  json_document["name"] = "Room";
  serializeJson(json_document, buffer);
  server.send(200, "application/json", buffer);
  digitalWrite(LED_PIN, LOW);
}

void getMeteoResponse() {
  digitalWrite(LED_PIN, HIGH);
  measurement_t* measurement = loadSensorData();
  StaticJsonDocument<SERIALIZED_MEASUREMENT_MAX_LENGTH> json_document;
  JsonObject json_object = json_document.to<JsonObject>();
  createJson(measurement, &json_object);
  char buffer[SERIALIZED_MEASUREMENT_MAX_LENGTH];
  serializeJson(json_document, buffer);
  server.send(200, "application/json", buffer);
  digitalWrite(LED_PIN, LOW);
}

void getHistoryResponse() {
  digitalWrite(LED_PIN, HIGH);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "application/json", "[");
  char buffer[SERIALIZED_MEASUREMENT_MAX_LENGTH] = { '\0' };
  DynamicJsonDocument json_document(SERIALIZED_MEASUREMENT_MAX_LENGTH * HISTORY_LENGTH);
  for (uint8_t idx = 0; idx < HISTORY_LENGTH; idx++) {
    if (history[idx] == NULL) {
      break;
    }
    memset(&buffer[0], 0, sizeof(buffer));
    StaticJsonDocument<SERIALIZED_MEASUREMENT_MAX_LENGTH> json_document;
    JsonObject json_object = json_document.to<JsonObject>();
    createJson(history[idx], &json_object);
    if (idx > 0) {
      server.sendContent(",");
    }
    serializeJson(json_document, buffer);
    server.sendContent(buffer);
  }
  server.sendContent("]");
  server.sendContent("");
  server.client().stop();
  digitalWrite(LED_PIN, LOW);
}

void setupRouting() {
  server.on("/", getMeteoResponse);
  server.on("/service", getServiceInfoResponse);
  server.on("/history", getHistoryResponse);
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

  Serial.print("Disabling Bluetooth module ...");
  if (btStop()) {
    Serial.println(" [ Done ]");
  } else {
    Serial.print(" [ Fail ]");
  }

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

  Serial.print("Setting up the real time clock ...");
  configTime(0, 0, "pool.ntp.org", "time.google.com", "time.cloudflare.com");
  while (now < 1687100000) {
    time(&now);
    Serial.print(".");
    delay(100);
  }
  Serial.println(" [ Done ]");

  Serial.print("Starting HTTP API server ...");
  setupRouting();
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