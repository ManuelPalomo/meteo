#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

void setup() {
  Serial.begin(9600);
  while (!Serial);

  //Built-in LED
  pinMode(LED_BUILTIN, OUTPUT);

  setupOLED();
  setupBME();
}

void readBMEValues() {
  temperature = bme.readTemperature();
  pressure = bme.readPressure() / 100;
  humidity = bme.readHumidity();
}

void readMQ135Values() {
  airQuality = analogRead(PIN_MQ135);
}

void printSerialValues() {
  Serial.println();

  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.print(" ºC");
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

void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("Hola!"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);

}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  readBMEValues();
  readMQ135Values();
  printSerialValues();
  digitalWrite(LED_BUILTIN, LOW);
  testscrolltext();
  delay(delayTimeMilliseconds);
}
