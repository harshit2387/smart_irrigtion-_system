#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <Preferences.h>
#include <BlynkSimpleEsp32.h>

// === CONFIGURATION ===
#define DHTPIN 15
#define DHTTYPE DHT11
#define SOIL_PIN 34
#define RELAY_PIN 26

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
char auth[] = "YOUR_BLYNK_AUTH";

const char* weatherAPI = "http://api.openweathermap.org/data/2.5/weather?q=YOUR_CITY&appid=YOUR_API_KEY";

// === OBJECTS ===
DHT dht(DHTPIN, DHTTYPE);
Preferences prefs;
BlynkTimer timer;

bool manualOverride = false;
bool manualState = false;

// === BLYNK CALLBACK ===
BLYNK_WRITE(V5) { // Manual override switch
  manualOverride = param.asInt();
}

BLYNK_WRITE(V6) { // Manual relay control
  manualState = param.asInt();
}

// === FUNCTIONS ===
bool isRainPredicted() {
  HTTPClient http;
  http.begin(weatherAPI);
  int code = http.GET();
  if (code == 200) {
    String payload = http.getString();
    return payload.indexOf("Rain") > 0;
  }
  http.end();
  return false;
}

void logMoisture(int value) {
  int index = prefs.getInt("index", 0);
  prefs.putInt(String(index).c_str(), value);
  prefs.putInt("index", index + 1);
}

void sendToBlynk(float temp, float hum, int soil, bool rain) {
  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, hum);
  Blynk.virtualWrite(V2, soil);
  Blynk.virtualWrite(V3, rain ? 1 : 0);
  Blynk.virtualWrite(V7, manualOverride ? "Manual" : "Auto");
}

void controlRelay(int soil, bool rain) {
  if (manualOverride) {
    digitalWrite(RELAY_PIN, manualState ? LOW : HIGH);
    Serial.println(manualState ? "Manual ON" : "Manual OFF");
  } else {
    if (soil < 2000 && !rain) {
      digitalWrite(RELAY_PIN, LOW); // ON
      Serial.println("Auto ON");
    } else {
      digitalWrite(RELAY_PIN, HIGH); // OFF
      Serial.println("Auto OFF");
    }
  }
}

void readAndAct() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int soil = analogRead(SOIL_PIN);
  bool rain = isRainPredicted();

  Serial.printf("Temp: %.1f°C, Hum: %.1f%%, Soil: %d, Rain: %d\n", temp, hum, soil, rain);
  logMoisture(soil);
  sendToBlynk(temp, hum, soil, rain);
  controlRelay(soil, rain);
}

// === SETUP ===
void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  dht.begin();
  prefs.begin("moistureLog", false);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Blynk.begin(auth, ssid, password);
  timer.setInterval(15000L, readAndAct);
}

// === LOOP ===
void loop() {
  Blynk.run();
  timer.run();
}