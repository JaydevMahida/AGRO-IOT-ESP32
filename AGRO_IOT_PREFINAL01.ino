#define BLYNK_TEMPLATE_ID "TMPL38f-VEtRQ"
#define BLYNK_TEMPLATE_NAME "SoilMoisturePrototype"
//
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#define SOIL_MOISTURE_PIN 34  // ADC pin for soil moisture sensor
#define PUMP_PIN 2 
char auth[] = "Qf27In2vuaqKvWM41PdCx4NE9UJ21xnO";
char ssid[] = "OnePlus Nord CE4";
char pass[] = "Jaydev007";

void setup() {
    Serial.begin(115200);  // Start serial communication
    Blynk.begin(auth, ssid, pass);
    pinMode(PUMP_PIN, OUTPUT); // Set LED pin as output
  digitalWrite(PUMP_PIN, LOW);
}

void loop() {
    int sensorValue = analogRead(SOIL_MOISTURE_PIN);  // Read analog value
    int moisturePercent = map(sensorValue, 0, 4095, 100, 0);  // Convert to percentage
    Serial.print("Soil Moisture: ");
    Serial.print(moisturePercent);
    Serial.println("%");
    Blynk.virtualWrite(V1, moisturePercent);  // Send to Blynk gauge
    delay(1000);
    Blynk.run();
}