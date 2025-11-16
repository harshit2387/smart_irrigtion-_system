#define BLYNK_TEMPLATE_ID "TMPL3ZF60qICP"
#define BLYNK_TEMPLATE_NAME "IOT"
#define BLYNK_AUTH_TOKEN "sPANDQ1N3B7LUzON2CFZlntPFjczIJj6"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// WiFi credentials
char ssid[] = "YASH";
char pass[] = "oooiiiiooo";

// Sensor and relay pins
#define DHTPIN 4
#define DHTTYPE DHT22
#define SOIL_PIN_1 34
#define SOIL_PIN_2 35
#define RELAY_PIN 23

// Blynk virtual pins
#define VPIN_TEMP V0
#define VPIN_HUM  V1
#define VPIN_AVG  V2
#define VPIN_SW   V3

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16x2 LCD

float temperature = 0;
float humidity = 0;
int soil1 = 0;
int soil2 = 0;

// Blynk switch handler
BLYNK_WRITE(VPIN_SW) {
  int switchState = param.asInt();
  digitalWrite(RELAY_PIN, switchState);
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Motor off initially

  dht.begin();
  lcd.init();
  lcd.backlight();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 2000) {
    lastUpdate = millis();
    updateSensors();
  }
}
void updateSensors() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  soil1 = analogRead(SOIL_PIN_1);
  soil2 = analogRead(SOIL_PIN_2);

  float avgTH = (temperature + humidity) / 2.0;
  float avgSoil = (soil1 + soil2) / 2.0;

  // Send to Blynk
  if (avgSoil < 60) {
    digitalWrite(RELAY_PIN, HIGH); // Turn ON pump
  } else {
    digitalWrite(RELAY_PIN, LOW);  // Turn OFF pump
  }
  Blynk.virtualWrite(VPIN_TEMP, temperature);
  Blynk.virtualWrite(VPIN_HUM, humidity);
  Blynk.virtualWrite(VPIN_AVG, avgTH);

  // Update LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  if (isnan(temperature)) {
    lcd.print("NaN");
  } else {
    lcd.print(temperature);
    lcd.print("C");
  }

  lcd.setCursor(0, 1);
  lcd.print("Hum:");
  if (isnan(humidity)) {
    lcd.print("NaN");
  } else {
    lcd.print(humidity);
    lcd.print("%");
  }

  // Optional: show soil moisture average
  lcd.setCursor(0, 2); // For 20x4 LCD; for 16x2, you can alternate display or scroll
  lcd.print("Soil Avg:");
  lcd.print(avgSoil, 0); // Rounded to integer

  // Serial debug
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" | Hum: ");
  Serial.print(humidity);
  Serial.print(" | Soil1: ");
  Serial.print(soil1);
  Serial.print(" | Soil2: ");
  Serial.print(soil2);
  Serial.print(" | SoilAvg: ");
  Serial.println(avgSoil);
}