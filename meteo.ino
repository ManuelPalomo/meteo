#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//BME
#define BME_SCK 19
#define BME_MOSI 18

#define SEALEVELPRESSURE_HPA (1025)

Adafruit_BME280 bme; // I2C

//MQ135
#define PIN_MQ135 A7

// Variables
float temperature;
float pressure;
float humidity;
float CO;
float CO2;

int delayTimeMilliseconds = 5000;

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
  while (!Serial);   // time to get serial running

  //Built-in LED
  pinMode(LED_BUILTIN, OUTPUT);

  setupBME();
}


void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  readBMEValues();
  readMQ135Values();
  printSerialValues();
  digitalWrite(LED_BUILTIN, LOW);
  delay(delayTimeMilliseconds);
}

void readBMEValues() {
  temperature = bme.readTemperature();
  pressure = bme.readPressure() / 100;
  humidity = bme.readHumidity();
}

void readMQ135Values(){
  CO2 = analogRead(PIN_MQ135);
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

  Serial.print("CO = ");
  Serial.print(CO);
  Serial.print(" ppm");
  Serial.println();


  Serial.print("CO2 = ");
  Serial.print(CO2);
  Serial.print(" ppm");
  Serial.println();


  Serial.println();

}
