/**
 *
 * HX711 library for Arduino - example file
 * https://github.com/bogde/HX711
 *
 * BMP280 library for Arduino
 * https://github.com/adafruit/Adafruit_BMP280_Library
 *
 * MIT License
 * (c) 2018 Bogdan Necula
 *
**/

#include <Arduino.h>
#include "HX711.h"
#include <Wire.h>
#include <Adafruit_BMP280.h>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

HX711 scale;

#define BMP280_ADDRESS 0x76
Adafruit_BMP280 bmp; // I2C

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(100); // wait for native usb
  Serial.println(F("HX711 and BMP280 test"));
  
  // Initializing the scale
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println(F("Before setting up the scale:"));
  Serial.print(F("read:\t\t"));
  Serial.println(scale.read()); // print a raw reading from the ADC

  Serial.print(F("read average:\t\t"));
  Serial.println(scale.read_average(20)); // print the average of 20 readings from the ADC

  Serial.print(F("get value:\t\t"));
  Serial.println(scale.get_value(5)); // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print(F("get units:\t\t"));
  Serial.println(scale.get_units(5), 1); // print the average of 5 readings from the ADC minus tare weight (not set) divided
                                          // by the SCALE parameter (not set yet)

  // Setting up the scale
  scale.set_scale(407);
  scale.tare(); // reset the scale to 0

  Serial.println(F("After setting up the scale:"));

  Serial.print(F("read:\t\t"));
  Serial.println(scale.read()); // print a raw reading from the ADC

  Serial.print(F("read average:\t\t"));
  Serial.println(scale.read_average(20)); // print the average of 20 readings from the ADC

  Serial.print(F("get value:\t\t"));
  Serial.println(scale.get_value(5)); // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print(F("get units:\t\t"));
  Serial.println(scale.get_units(5), 1); // print the average of 5 readings from the ADC minus tare weight, divided
                                          // by the SCALE parameter set with set_scale

  Serial.println(F("BMP280 Initializing..."));
  unsigned status;
  status = bmp.begin(BMP280_ADDRESS);
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                     "try a different address!"));
    Serial.print(F("SensorID was: 0x"));
    Serial.println(bmp.sensorID(), 16);
    Serial.print(F("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n"));
    Serial.print(F("   ID of 0x56-0x58 represents a BMP 280,\n"));
    Serial.print(F("        ID of 0x60 represents a BME 280.\n"));
    Serial.print(F("        ID of 0x61 represents a BME 680.\n"));
    while (1)
      delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2, /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16, /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16, /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void loop() {
  Serial.print(F("HX711: one reading:\t"));
  Serial.print(scale.get_units(), 1);
  Serial.print(F("\t| average:\t"));
  Serial.println(scale.get_units(10), 5);

  Serial.print(F("BMP280: Temperature = "));
  Serial.print(bmp.readTemperature());
  Serial.print(F(" *C | "));

  Serial.print(F("Pressure = "));
  Serial.print(bmp.readPressure());
  Serial.print(F(" Pa | "));

  Serial.print(F("Approx altitude = "));
  Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
  Serial.print(F(" m | "));

  Serial.println();
  delay(5000);
}
