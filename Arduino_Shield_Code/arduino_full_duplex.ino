#include <Arduino.h>
#include "HX711.h"
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <SoftwareSerial.h>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;
const int PIR_PIN = 4; // PIR sensor pin

HX711 scale;

#define BMP280_ADDRESS 0x76
Adafruit_BMP280 bmp; // I2C

SoftwareSerial serialPort(6, 7); // SoftwareSerial object using pins 6 (RX) and 7 (TX)

unsigned long previousMillis = 0;
const long interval = 2000; // 2 seconds interval

void setup() {
  Serial.begin(9600); // Initialize hardware serial (USB)
  serialPort.begin(9600); // Initialize software serial
  pinMode(PIR_PIN, INPUT); // Set PIR pin as input

  Serial.println(F("HX711, BMP280 and PIR test")); // Print to USB
  serialPort.println(F("HX711, BMP280 and PIR test")); // Print to software serial

  // Initializing the scale
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // Setting up the scale
  scale.set_scale(407);
  scale.tare(); // reset the scale to 0

  // Initializing BMP280
  Serial.println(F("BMP280 Initializing...")); // Print to USB
  serialPort.println(F("BMP280 Initializing...")); // Print to software serial
  unsigned status = bmp.begin(BMP280_ADDRESS);
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                     "try a different address!")); // Print to USB
    serialPort.println(F("Could not find a valid BMP280 sensor, check wiring or "
                     "try a different address!")); // Print to software serial
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
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Read PIR sensor
    int pirState = digitalRead(PIR_PIN);

    if (pirState == HIGH) {
      Serial.println(F("PIR: Motion Detected")); // Print to USB
      serialPort.println(F("PIR: Motion Detected")); // Print to software serial
      // Add your actions here when motion is detected
    } else {
      Serial.println(F("PIR: Motion Not Detected")); // Print to USB
      serialPort.println(F("PIR: Motion Not Detected")); // Print to software serial
      // Add your actions here when no motion is detected
    }

    Serial.print(F("HX711: one reading:\t")); // Print to USB
    serialPort.print(F("HX711: one reading:\t")); // Print to software serial
    Serial.print(scale.get_units(), 1); // print the reading to USB
    serialPort.print(scale.get_units(), 1); // print the reading to software serial
    Serial.print(F("\t| average:\t")); // Print to USB
    serialPort.print(F("\t| average:\t")); // Print to software serial
    Serial.println(scale.get_units(10), 5); // print the average to USB
    serialPort.println(scale.get_units(10), 5); // print the average to software serial

    Serial.print(F("BMP280: Temperature = ")); // Print to USB
    serialPort.print(F("BMP280: Temperature = ")); // Print to software serial
    Serial.print(bmp.readTemperature()); // print temperature to USB
    serialPort.print(bmp.readTemperature()); // print temperature to software serial
    Serial.print(F(" *C | ")); // Print to USB
    serialPort.print(F(" *C | ")); // Print to software serial

    Serial.print(F("Pressure = ")); // Print to USB
    serialPort.print(F("Pressure = ")); // Print to software serial
    Serial.print(bmp.readPressure()); // print pressure to USB
    serialPort.print(bmp.readPressure()); // print pressure to software serial
    Serial.print(F(" Pa | ")); // Print to USB
    serialPort.print(F(" Pa | ")); // Print to software serial

    Serial.print(F("Approx altitude = ")); // Print to USB
    serialPort.print(F("Approx altitude = ")); // Print to software serial
    Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
    serialPort.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
    Serial.print(F(" m | ")); // Print to USB
    serialPort.print(F(" m | ")); // Print to software serial

    Serial.println(); // Print to USB
    serialPort.println(); // Print to software serial
  }

  // Read serial messages from computer
  if (serialPort.available() > 0) {
    String serialMessage = serialPort.readStringUntil('\n');

    // Pass the message to the other MCU by writing it to TX pin
    Serial.println(serialMessage); // Print to USB
    serialPort.println(serialMessage); // Print to software serial
    Serial.println("Serial_Software"); // Print to USB
    serialPort.println("Serial_Software"); // Print to software serial
  }

  // Read serial messages from computer
  if (Serial.available() > 0) {
    String serialMessage1 = Serial.readStringUntil('\n');

    // Pass the message to the other MCU by writing it to TX pin
    Serial.println(serialMessage1); // Print to USB
    serialPort.println(serialMessage1); // Print to software serial
    Serial.println("Serial_USB"); // Print to USB
    serialPort.println("Serial_USB"); // Print to software serial
  }
}
