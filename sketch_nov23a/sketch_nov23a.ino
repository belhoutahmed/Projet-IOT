#include <TFT_eSPI.h>  // Include library for the TFT screen
#include <WiFi.h>      // Include Wi-Fi library
#include <HTTPClient.h> // Include HTTP client library

TFT_eSPI tft = TFT_eSPI();  // Initialize the TFT screen

// Wi-Fi credentials
const char* ssid = "Mi 11";          // Wi-Fi network name
const char* password = "87654321";   // Wi-Fi password

// Backend URL
const char* serverURL = "http://192.168.155.210:3000/";  //  backend server URL

// Pin definitions
const int photoTransistorPin = 32; // GPIO for the photo-transistor
const int tempSensorPin = 33;      // GPIO for the LM35 temperature sensor
const int ledPin = 22;             // GPIO for the LED

// Thresholds
const float tempThreshold = 30.0;  // Temperature threshold (in °C) to turn on the LED
const int lightThreshold = 2000;   // Light threshold to turn on the LED

void connectToBackend(float temperature, int lightLevel) {
  if (WiFi.status() == WL_CONNECTED) {  // Check if Wi-Fi is connected
    HTTPClient http;
    http.begin(serverURL);

    String payload = String("{\"temperature\":") + temperature + ",\"lightLevel\":" + lightLevel + "}";
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server Response: " + response);

      // Display response on TFT
      tft.fillScreen(TFT_BLUE);
      tft.setTextColor(TFT_WHITE, TFT_BLUE);
      tft.setTextSize(2);
      tft.setCursor(10, 10);
      tft.println("Server OK");

      // Log to Serial
      Serial.println("Server Status: OK");
    } else {
      tft.fillScreen(TFT_RED);
      tft.setTextColor(TFT_WHITE, TFT_RED);
      tft.setTextSize(2);
      tft.setCursor(10, 10);
      tft.println("Server Error");

      // Log to Serial
      Serial.println("Server Error");
    }
    http.end();
  } else {
    Serial.println("Wi-Fi not connected");
  }
}

void setup() {
  Serial.begin(115200);  // Start serial communication
  tft.init();            // Initialize the TFT screen
  tft.setRotation(1);    // Set screen orientation to landscape
  tft.fillScreen(TFT_BLACK);

  pinMode(photoTransistorPin, INPUT); // Configure photo-transistor pin as input
  pinMode(tempSensorPin, INPUT);      // Configure LM35 pin as input
  pinMode(ledPin, OUTPUT);            // Configure LED pin as output

  // Display initialization message
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("System Initializing");
  Serial.println("System Initializing...");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Read photo-transistor value
  int lightLevel = analogRead(photoTransistorPin);

  // Read temperature sensor value
  int tempValue = analogRead(tempSensorPin);
  float voltage = tempValue * (3.3 / 4095.0); // Convert ADC value to voltage
  float temperature = voltage * 100.0;       // Convert voltage to temperature (LM35: 10mV/°C)

  // Display values on TFT
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10, 10);
  tft.setTextSize(2);
  tft.printf("Temp: %.2f C\n", temperature);
  tft.printf("Light: %d\n", lightLevel);

  // Log values to Serial
  Serial.printf("Temperature: %.2f C\n", temperature);
  Serial.printf("Light Level: %d\n", lightLevel);

  // Control LED based on thresholds
  if (temperature > tempThreshold || lightLevel < lightThreshold) {
    digitalWrite(ledPin, HIGH);  // Turn on LED
    tft.println("LED: ON");
    Serial.println("LED Status: ON");
  } else {
    digitalWrite(ledPin, LOW);   // Turn off LED
    tft.println("LED: OFF");
    Serial.println("LED Status: OFF");
  }

  // Send data to the backend
  connectToBackend(temperature, lightLevel);

  delay(5000); // Wait for 5 seconds before refreshing
}
