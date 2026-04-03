#define BLYNK_TEMPLATE_ID "TMPL3h20mmXfP"
#define BLYNK_TEMPLATE_NAME "AGRO IOT"
#define BLYNK_AUTH_TOKEN "scX6kHzsQrzlqmRD2TtfMDm-xW5CJ6RG"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// ===== WIFI =====
char ssid[] = "OnePlus Nord CE4";
char pass[] = "Jaydev007";

// ===== PINS =====
#define DHTPIN D4
#define DHTTYPE DHT11
#define SOIL_PIN A0
#define RELAY_PIN D1

// ===== RELAY LOGIC (change if needed) =====
#define RELAY_ON LOW
#define RELAY_OFF HIGH

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// ===== VARIABLES =====
int pumpState = 0; // 0=OFF, 1=ON

// ===== SENSOR FUNCTION =====
void sendData() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int soilRaw = analogRead(SOIL_PIN);

  if (isnan(temp) || isnan(hum)) {
    temp = 0;
    hum = 0;
  }

  // Soil %
  int soilPercent = map(soilRaw, 1023, 300, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  // ===== SEND TO BLYNK =====
  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, hum);
  Blynk.virtualWrite(V2, soilPercent);

  // ===== SERIAL OUTPUT =====
  Serial.println("------ SENSOR DATA ------");
  Serial.print("Temperature: "); Serial.print(temp); Serial.println(" C");
  Serial.print("Humidity: "); Serial.print(hum); Serial.println(" %");
  Serial.print("Soil Raw: "); Serial.println(soilRaw);
  Serial.print("Soil %: "); Serial.print(soilPercent); Serial.println(" %");

  if (pumpState == 1) {
    Serial.println("Pump State: ON");
  } else {
    Serial.println("Pump State: OFF");
  }
  Serial.println("-------------------------\n");
}

// ===== PUMP CONTROL =====
BLYNK_WRITE(V3) {
  pumpState = param.asInt();

  Serial.print("Button Pressed: ");
  Serial.println(pumpState);

  if (pumpState == 1) {
    digitalWrite(RELAY_PIN, RELAY_ON);
    Serial.println("Pump turned ON");
  } else {
    digitalWrite(RELAY_PIN, RELAY_OFF);
    Serial.println("Pump turned OFF");
  }
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF);

  dht.begin();

  // ===== STABLE WIFI =====
  WiFi.mode(WIFI_STA);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);

  WiFi.begin(ssid, pass);

  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // ===== BLYNK CONNECT =====
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();

  // ===== TIMER =====
  timer.setInterval(2000L, sendData);
}

// ===== LOOP =====
void loop() {
  Blynk.run();
  timer.run();

  // Auto reconnect WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting WiFi...");
    WiFi.begin(ssid, pass);
    delay(2000);
  }
}