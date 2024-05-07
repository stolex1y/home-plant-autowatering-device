/*
#include <Arduino.h>
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }  // Waiting for serial connection

  Serial.println();
  Wire.begin(D2, D1);
  Serial.println("Start I2C scanner ...");
}

void loop() {
  delay(5000);
  Wire.beginTransmission(0x23);
  if (Wire.endTransmission() == 0) {
    Serial.println("Found I2C Device");
  } else {
    Serial.println("not found");
  }
}
*/
