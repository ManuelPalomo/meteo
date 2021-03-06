#include "secrets.h"
#include <WiFiNINA.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


//WifiNINA
int status = WL_IDLE_STATUS;
int wifiRSSI;
String wifiRSSIQuality = "Not connected";

//BME
#define BME_SCK 19
#define BME_MOSI 18

#define SEALEVELPRESSURE_HPA (1025)

Adafruit_BME280 bme; // I2C

//MQ135
#define PIN_MQ135 A7

//OLED display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables
float temperature;
float pressure;
float humidity;
float airQuality;

int delayTimeMilliseconds = 5000;

void setupOLED() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
}

void setupBME() {
  bme.begin();

  bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                  Adafruit_BME280::SAMPLING_X2,  // temperature
                  Adafruit_BME280::SAMPLING_X16, // pressure
                  Adafruit_BME280::SAMPLING_X1,  // humidity
                  Adafruit_BME280::FILTER_X16,
                  Adafruit_BME280::STANDBY_MS_0_5 );
}

void connectWifi() {
  int attempts = 0;
  while (status != WL_CONNECTED && attempts < 2) {
    Serial.println("Attempting to connect Wifi: Attempt " + String(attempts));
    attempts += 1;
    status = WiFi.begin(WIFI_SSID, WIFI_PASS);
    delay(5000);
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  //Built-in LED
  pinMode(LED_BUILTIN, OUTPUT);

  setupOLED();
  setupBME();
  connectWifi();
}

void readBMEValues() {
  temperature = bme.readTemperature();
  pressure = bme.readPressure() / 100;
  humidity = bme.readHumidity();
}

void readMQ135Values() {
  airQuality = analogRead(PIN_MQ135);
}

void readWifiStatus() {
  if (WiFi.status() == WL_CONNECTED) {
    wifiRSSI = WiFi.RSSI();
    wifiRSSIQuality = translateRSSIToValue(wifiRSSI);
  }
}

String translateRSSIToValue(int wifiRSSI) {
  if (wifiRSSI > -60) return "Good";
  if (wifiRSSI < -60 && wifiRSSI > -70) return "Medium";
  if (wifiRSSI < -80) return "Low/Bad";
}

void printSerialValues() {
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Wifi signal strength: ");
    Serial.print(wifiRSSI);
    Serial.print(" " + wifiRSSIQuality);
  } else {
    Serial.print("Wifi disconnected");
  }
  
  Serial.println();

  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.print(" ??C");
  Serial.println();

  Serial.print("Pressure = ");
  Serial.print(0.02953 * pressure);
  Serial.print(" inHg");
  Serial.println();

  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.print(" %");
  Serial.println();

  Serial.print("Air Quality = ");
  Serial.print(airQuality);
  Serial.print(" ppm");
  Serial.println();

  Serial.println();
}

void displayData(void) {
  float pressureInHg = 0.02952 * pressure;

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  drawFrame();

  display.setCursor(3, 3);
  display.println(String(temperature, 2) + char(247) + "C");

  display.setCursor(3, 12);
  display.println(String(pressureInHg, 2) + " mmHg" + "/" + String(pressure, 2) + " Pa");

  display.setCursor(3, 21);
  display.println(String(humidity, 2) + " % Humidity");

  display.setCursor(3, 30);

  display.println(String(airQuality, 2) + " ppm in air");

  display.setCursor(3, 39);
  display.println("Air Quality: " + getAirQualityString(airQuality));

  display.setCursor(3, 48);
  if (WiFi.status() == WL_CONNECTED) {
    display.println("Wifi: " + String(wifiRSSI) + " " + wifiRSSIQuality);
  } else {
    display.println("Wifi: Not connected");
  }

  display.display();
}

void drawFrame() {
  display.drawLine(0, 0, SCREEN_WIDTH - 1, 0, 1);
  display.drawLine(0, 0, 0, SCREEN_HEIGHT - 1, 1);
  display.drawLine(0, SCREEN_HEIGHT - 1, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, 1);
  display.drawLine(SCREEN_WIDTH - 1, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, 1);
}

String getAirQualityString(float airQualityPpm) {
  if (airQualityPpm < 750) return "Good";
  if (airQualityPpm > 750 && airQualityPpm < 1200) return "Not good";
  if (airQualityPpm > 1200) return "Bad";
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  readBMEValues();
  readMQ135Values();
  readWifiStatus();
  printSerialValues();
  digitalWrite(LED_BUILTIN, LOW);
  displayData();
  delay(delayTimeMilliseconds);
}
