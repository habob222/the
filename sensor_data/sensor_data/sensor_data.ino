#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiClient.h>

// --- WiFi Setup ---
const char* ssid = "OPPO Reno8 T";  // Your WiFi SSID
const char* password = "55555555";  // Your WiFi Password

// --- Server Setup ---
const char* serverHost = "192.168.77.173";  // Server IP or hostname
const int serverPort = 5000;  // Server port

// --- Pressure Sensor Setup ---
#define SENSOR_PIN 17  
const float sensitivity = 0.1;  
const float offset = 0.5;      


// --- VOC Sensor Pins ---
const int mq3Pin = 34;   
const int mq2Pin = 35;    
const int mq135Pin = 26;  // MQ-135 sensor connected to GPIO 26

// --- VOC Calibration Factors ---
const float mq3_ethanol_factor = 0.15;
const float mq3_acetone_factor = 0.18;
const float mq3_isopropanol_factor = 0.14;
const float mq3_hexane_factor = 0.12;
const float mq3_methanol_factor = 0.10;
const float mq3_butane_factor = 0.11;
const float mq3_pentane_factor = 0.13;

const float mq2_smoke_factor = 0.12;
const float mq2_methane_factor = 0.10;
const float mq2_propane_factor = 0.15;
const float mq2_co_factor = 0.25;

const float mq135_benzene_factor = 0.20;
const float mq135_toluene_factor = 0.18;
const float mq135_ethylbenzene_factor = 0.17;
const float mq135_xylene_factor = 0.16;
const float mq135_naphthalene_factor = 0.15;
const float mq135_dimethyl_sulfide_factor = 0.22;
const float mq135_styrene_factor = 0.19;
const float mq135_methyl_mercaptan_factor = 0.21;

// --- DHT11 Setup ---
#define DHTPIN 4       // Pin connected to the DATA pin of the DHT11 sensor
#define DHTTYPE DHT11   // Define the type of DHT sensor (DHT11 in this case)
DHT dht(DHTPIN, DHTTYPE);  // Initialize the DHT sensor

WiFiClient client;

void setup() {
  Serial.begin(115200);  // Start Serial communication at 115200 baud

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to WiFi");

  pinMode(SENSOR_PIN, INPUT);  // Set the pressure sensor pin as input
  dht.begin();  // Initialize the DHT11 sensor
}

void loop() {
  // --- Pressure Sensor Reading ---
  int rawValue = analogRead(SENSOR_PIN);  // Read the analog value from the pressure sensor
  float voltage = (rawValue / 4095.0) * 3.3;  // Convert raw ADC value to voltage (ESP32 has 12-bit ADC)
  float pressure = (voltage - offset) / sensitivity;  // Apply calibration (adjust sensitivity and offset)

  // --- VOC Sensor Readings ---
  int mq3Value = analogRead(mq3Pin);    // Read value from MQ-3
  int mq2Value = analogRead(mq2Pin);    // Read value from MQ-2
  int mq135Value = analogRead(mq135Pin);  // Read value from MQ-135

  // Calculate approximate concentrations in PPM for each VOC
  float mq3Ethanol = mq3Value * mq3_ethanol_factor;
  float mq3Acetone = mq3Value * mq3_acetone_factor;
  float mq3Isopropanol = mq3Value * mq3_isopropanol_factor;
  float mq3Hexane = mq3Value * mq3_hexane_factor;
  float mq3Methanol = mq3Value * mq3_methanol_factor;
  float mq3Butane = mq3Value * mq3_butane_factor;
  float mq3Pentane = mq3Value * mq3_pentane_factor;

  float mq2Smoke = mq2Value * mq2_smoke_factor;
  float mq2Methane = mq2Value * mq2_methane_factor;
  float mq2Propane = mq2Value * mq2_propane_factor;
  float mq2CO = mq2Value * mq2_co_factor;
  float mq135Benzene = mq135Value * mq135_benzene_factor;
  float mq135Toluene = mq135Value * mq135_toluene_factor;
  float mq135Ethylbenzene = mq135Value * mq135_ethylbenzene_factor;
  float mq135Xylene = mq135Value * mq135_xylene_factor;
  float mq135Naphthalene = mq135Value * mq135_naphthalene_factor;
  float mq135DimethylSulfide = mq135Value * mq135_dimethyl_sulfide_factor;
  float mq135Styrene = mq135Value * mq135_styrene_factor;
  float mq135MethylMercaptan = mq135Value * mq135_methyl_mercaptan_factor;                                         

  // --- DHT11 Sensor Reading ---
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Temperature in Celsius
  // Check if DHT11 reading failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // --- Create JSON Data ---
  String postData = "{";
  postData += "\"pressure\": " + String(pressure) + ",";  
  postData += "\"mq3\": {\"ethanol\": " + String(mq3Ethanol) + ", \"acetone\": " + String(mq3Acetone) + ", \"isopropanol\": " + String(mq3Isopropanol) + ", \"hexane\": " + String(mq3Hexane) + ", \"methanol\": " + String(mq3Methanol) + ", \"butane\": " + String(mq3Butane) + ", \"pentane\": " + String(mq3Pentane) + "},";  
  postData += "\"mq2\": {\"smoke\": " + String(mq2Smoke) + ", \"methane\": " + String(mq2Methane) + ", \"propane\": " + String(mq2Propane) + ", \"co\": " + String(mq2CO) + "},";  
  postData += "\"mq135\": {\"benzene\": " + String(mq135Benzene) + ", \"toluene\": " + String(mq135Toluene) + ", \"ethylbenzene\": " + String(mq135Ethylbenzene) + ", \"xylene\": " + String(mq135Xylene) + ", \"naphthalene\": " + String(mq135Naphthalene) + ", \"dimethyl_sulfide\": " + String(mq135DimethylSulfide) + ", \"styrene\": " + String(mq135Styrene) + ", \"methyl_mercaptan\": " + String(mq135MethylMercaptan) + "},";  
  postData += "\"humidity\": " + String(humidity) + ",";  
  postData += "\"temperature\": " + String(temperature);
  postData += "}";

  // --- Print the data to Serial Monitor for Debugging ---
  Serial.println("Data to send:");
  Serial.println(postData);

  // --- Send Data to Server ---
  if (WiFi.status() == WL_CONNECTED) {
    if (client.connect(serverHost, serverPort)) {
      Serial.println("Connected to server");

      // Construct the HTTP POST request
      client.println("POST /upload HTTP/1.1");
      client.println("Host: " + String(serverHost));
      client.println("Content-Type: application/json");
      client.println("Content-Length: " + String(postData.length()));
      client.println(); // End of headers
      client.println(postData); // Request body

      // Check HTTP response
      while (client.connected()) {
        if (client.available()) {
          String response = client.readStringUntil('\n');
          Serial.println("Response: " + response);
          break;
        }
      }

      // Close the connection
      client.stop();
    } else {
      Serial.println("Connection to server failed");
    }
  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(10000);  // Post every 10 seconds
}