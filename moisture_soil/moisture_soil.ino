#define SOIL_PIN_1 34
#define SOIL_PIN_2 35

#define BLYNK_TEMPLATE_ID "TMPL3ZF60qICP"
#define BLYNK_TEMPLATE_NAME "IOT"
#define BLYNK_AUTH_TOKEN "sPANDQ1N3B7LUzON2CFZlntPFjczIJj6"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "YASH";
char pass[] = "oooiiiiooo";

void setup() {
  Serial.begin(115200);
  delay(1000); // Give time for Serial to initialize

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();

  int raw1 = analogRead(SOIL_PIN_1);
  int raw2 = analogRead(SOIL_PIN_2);

  // Convert raw values to percentage (0 = wet, 100 = dry)
  int moisture1 = map(raw1, 4095, 0, 0, 100);
  int moisture2 = map(raw2, 4095, 0, 0, 100);
  int avgMoisture = (moisture1 + moisture2) / 2;

  Serial.print("Soil Moisture 1 (%): ");
  Serial.print(moisture1);
  Serial.print(" | Soil Moisture 2 (%): ");
  Serial.print(moisture2);
  Serial.print(" | Average Moisture (%): ");
  Serial.println(avgMoisture);

  Blynk.virtualWrite(V2, avgMoisture);  // Send average to Blynk V3

  delay(2000); // Wait 2 seconds before next reading
}
