#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Plant Monitoring System"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// WiFi Credentials
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";
char auth[] = BLYNK_AUTH_TOKEN;

// Relay Configuration
#define RELAY_PIN D0
#define RELAY_ACTIVE_HIGH false

// Sensors
#define DHT_PIN   D3
#define DHT_TYPE  DHT11
#define SOIL_PIN  A0

// LCD Configuration
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2
#define SDA_PIN D2
#define SCL_PIN D1

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
DHT dht(DHT_PIN, DHT_TYPE);

// Variables
bool pumpManual = false;
bool pumpState = false;
int autoThreshold = 40;
float soilPercent = 0;
float soilAlpha = 0.2;
bool wifiConnected = false;
bool offlineMode = false;

BlynkTimer timer;
const unsigned long READ_INTERVAL = 5000;

// Alert Thresholds
const float TEMP_HIGH_LIMIT = 35.0;
const float HUM_LOW_LIMIT   = 30.0;
const int   SOIL_LOW_LIMIT  = 30;

bool tempAlertSent = false;
bool humAlertSent  = false;
bool soilAlertSent = false;

// ---------------- BLYNK CONTROL ----------------
BLYNK_WRITE(V3) {
  int v = param.asInt();
  pumpManual = (v == 1);
  setPump(pumpManual);
}

BLYNK_WRITE(V5) {
  int v = param.asInt();
  if (v >= 0 && v <= 100) autoThreshold = v;
}

// ---------------- SOIL SENSOR ----------------
int readSoilRaw() {
  return analogRead(SOIL_PIN);
}

int mapSoilToPercent(int raw) {
  int dryRaw = 1023;
  int wetRaw = 350;
  raw = constrain(raw, wetRaw, dryRaw);
  int perc = map(raw, dryRaw, wetRaw, 0, 100);
  return constrain(perc, 0, 100);
}

// ---------------- RELAY CONTROL ----------------
void setPump(bool on) {
  if (RELAY_ACTIVE_HIGH) digitalWrite(RELAY_PIN, on ? HIGH : LOW);
  else digitalWrite(RELAY_PIN, on ? LOW : HIGH);

  pumpState = on;
  if (wifiConnected) Blynk.virtualWrite(V4, pumpState);
}

// ---------------- AUTO CONTROL ----------------
void checkAutoControl(int soilP) {
  if (pumpManual) return;

  if (soilP < autoThreshold) {
    setPump(true);
  } else {
    setPump(false);
  }
}

// ---------------- ALERTS ----------------
void checkAlerts(float temp, float hum, int soil) {
  if (!wifiConnected) return;

  if (temp > TEMP_HIGH_LIMIT && !tempAlertSent) {
    Blynk.logEvent("high_temp", "High Temperature Alert");
    tempAlertSent = true;
  } else if (temp <= TEMP_HIGH_LIMIT - 2) tempAlertSent = false;

  if (hum < HUM_LOW_LIMIT && !humAlertSent) {
    Blynk.logEvent("low_humidity", "Low Humidity Alert");
    humAlertSent = true;
  } else if (hum >= HUM_LOW_LIMIT + 5) humAlertSent = false;

  if (soil < SOIL_LOW_LIMIT && !soilAlertSent) {
    Blynk.logEvent("low_soil", "Low Soil Moisture Alert");
    soilAlertSent = true;
  } else if (soil >= SOIL_LOW_LIMIT + 10) soilAlertSent = false;
}

// ---------------- SEND DATA ----------------
void sendToBlynk(int soilP, float t, float h) {
  if (!wifiConnected) return;

  Blynk.virtualWrite(V0, soilP);
  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, h);
  Blynk.virtualWrite(V5, autoThreshold);
  Blynk.virtualWrite(V4, pumpState);
}

// ---------------- LCD DISPLAY ----------------
void updateLCD(int soil, float temp, float hum, bool pump) {
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Soil:");
  lcd.print(soil);
  lcd.print("% ");
  lcd.print(pump ? "ON" : "OFF");

  lcd.setCursor(0,1);
  lcd.print("T:");
  lcd.print(temp,1);
  lcd.print("C H:");
  lcd.print(hum,1);
  lcd.print("%");
}

// ---------------- SENSOR READING ----------------
void readSensors() {
  int raw = readSoilRaw();
  int soilP = mapSoilToPercent(raw);
  soilPercent = (soilAlpha * soilP) + (1.0 - soilAlpha) * soilPercent;

  float hum = dht.readHumidity();
  float temp = dht.readTemperature();

  checkAutoControl((int)soilPercent);
  sendToBlynk((int)soilPercent, temp, hum);
  updateLCD((int)soilPercent, temp, hum, pumpState);
  checkAlerts(temp, hum, (int)soilPercent);
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_ACTIVE_HIGH ? LOW : HIGH);

  dht.begin();
  Wire.begin(SDA_PIN, SCL_PIN);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Plant Monitor");

  WiFi.begin(ssid, pass);

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Blynk.begin(auth, ssid, pass);
  } else {
    offlineMode = true;
  }

  timer.setInterval(READ_INTERVAL, readSensors);
}

// ---------------- LOOP ----------------
void loop() {
  if (wifiConnected && WiFi.status() == WL_CONNECTED) {
    Blynk.run();
  }
  timer.run();
}
