/*
#include <Arduino.h>
#include <Wire.h>

#include "logger.h"
#include "sensors/soil_moisture_sensor.h"

using namespace hpa;

sensors::SoilMoistureSensor soil_moisture_sensor(A0);

void setup() {
  if (LOG_LEVEL < static_cast<int>(logger::LogLevel::kNone)) {
    Serial.begin(115200);
    Serial.println();
  }
  delay(5000);
  ESP.deepSleep(0);
}

void loop() {
  LOG_DEBUG("soil moisture: %f", soil_moisture_sensor.GetSoilMoisture());
  delay(1000);
}
*/
