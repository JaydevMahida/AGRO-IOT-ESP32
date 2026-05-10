#define BLYNK_TEMPLATE_ID "TMPL3h20mmXfP"
#define BLYNK_TEMPLATE_NAME "AGRO IOT"
#define BLYNK_AUTH_TOKEN "scX6kHzsQrzlqmRD2TtfMDm-xW5CJ6RG"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// ================= WIFI =================
char ssid[] = "Agro";
char pass[] = "Jaydev007";

// ================= PINS =================
#define DHTPIN D4
#define DHTTYPE DHT11
#define SOIL_PIN A0
#define RELAY_PIN D1

#define RELAY_ON LOW
#define RELAY_OFF HIGH

// ================= OBJECTS =================
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// ================= VARIABLES =================
int pumpState = 0;

// Soil calibration
int dryValue = 1023;
int wetValue = 300;

// Sensor calibration
float tempOffset = 6.5;     // Adjusted for real temperature
float humOffset  = -78.0;   // Adjusted for real humidity

// ===================================================
// SENSOR FUNCTION
// ===================================================
void sendData() {

  // Read DHT values
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // Read soil sensor
  int soilRaw = analogRead(SOIL_PIN);

  // Check sensor
  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT ERROR!");
    return;
  }

  // ================= CALIBRATION =================
  temp = temp + tempOffset;
  hum  = hum + humOffset;

  // Limit humidity
  hum = constrain(hum, 0, 100);

  // ================= SOIL MAPPING =================
  int soilPercent = map(soilRaw, dryValue, wetValue, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  // ================= SEND TO BLYNK =================
  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, hum);
  Blynk.virtualWrite(V2, soilPercent);

  // ================= SERIAL MONITOR =================
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println(" %");

  Serial.print("Soil Moisture: ");
  Serial.print(soilPercent);
  Serial.println(" %");

  Serial.println("----------------------------");
}

// ===================================================
// PUMP CONTROL FROM BLYNK
// ===================================================
BLYNK_WRITE(V3) {

  pumpState = param.asInt();

  if (pumpState == 1) {
    digitalWrite(RELAY_PIN, RELAY_ON);
    Serial.println("Pump ON");
  }
  else {
    digitalWrite(RELAY_PIN, RELAY_OFF);
    Serial.println("Pump OFF");
  }
}

// ===================================================
// SETUP
// ===================================================
void setup() {

  Serial.begin(115200);

  // Relay setup
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF);

  // Start DHT
  dht.begin();

  // Connect Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Sync app state
  Blynk.syncAll();

  // Send data every 2 seconds
  timer.setInterval(2000L, sendData);

  Serial.println("AGRO IOT SYSTEM STARTED");
}

// ===================================================
// LOOP
// ===================================================
void loop() {

  Blynk.run();
  timer.run();
}
