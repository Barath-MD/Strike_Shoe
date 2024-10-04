#include <DHT.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <WiFi.h>
#include <ThingSpeak.h>

// DHT Sensor
#define DHTPIN 4          // Pin for DHT sensor
#define DHTTYPE DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// BMP180 Sensors
Adafruit_BMP085_Unified bmp1 = Adafruit_BMP085_Unified(10085);
Adafruit_BMP085_Unified bmp2 = Adafruit_BMP085_Unified(10086);
Adafruit_BMP085_Unified bmp3 = Adafruit_BMP085_Unified(10087);

// Moisture Sensor
#define MOISTUREPIN 34    // Analog pin for moisture sensor

// Wi-Fi credentials
const char* ssid = "Loki";
const char* password = "lokesh11";

// ThingSpeak credentials
const char* apiKey = "LP2TQ52MOVFIKLY9";
unsigned long channelID =  2683036;

void setup() {
  Serial.begin(115200);
  
  // Initialize DHT sensor
  dht.begin();
  
  // Initialize BMP180 sensors
  if (!bmp1.begin() || !bmp2.begin() || !bmp3.begin()) {
    Serial.println("Could not find a valid BMP180 sensor, check wiring!");
    while (1);
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  
  ThingSpeak.begin(WiFi);
}

void loop() {
  // Reading DHT sensor for temperature and humidity
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Reading BMP180 for pressure
  sensors_event_t event1, event2, event3;
  bmp1.getEvent(&event1);
  bmp2.getEvent(&event2);
  bmp3.getEvent(&event3);
  
  float pressure1 = (event1.pressure) ? event1.pressure : 0; // Handle error
  float pressure2 = (event2.pressure) ? event2.pressure : 0; // Handle error
  float pressure3 = (event3.pressure) ? event3.pressure : 0; // Handle error

  // Reading moisture sensor
  int moistureLevel = analogRead(MOISTUREPIN);

  // Check if any reads failed
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Prepare data for ThingSpeak
  ThingSpeak.setField(1, temperature);  // Temperature
  ThingSpeak.setField(2, humidity);     // Humidity
  ThingSpeak.setField(3, pressure1);    // Pressure from Sensor 1
  ThingSpeak.setField(4, pressure2);    // Pressure from Sensor 2
  ThingSpeak.setField(5, pressure3);    // Pressure from Sensor 3
  ThingSpeak.setField(6, moistureLevel); // Moisture Level

  // Write to ThingSpeak
  int responseCode = ThingSpeak.writeFields(channelID, apiKey);
  if (responseCode == 200) {
    Serial.println("Data sent successfully.");
  } else {
    Serial.print("Error sending data. HTTP response code: ");
    Serial.println(responseCode);
  }

  delay(20000); // Update every 20 seconds
}
