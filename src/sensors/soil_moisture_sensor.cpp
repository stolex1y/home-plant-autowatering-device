#include "sensors/soil_moisture_sensor.h"

#include "logger.h"

namespace hpa::sensors {

SoilMoistureSensor::SoilMoistureSensor(pins::Pin::PinNumber pin_number)
    : AnalogSensor(pin_number, kMin, kMax, true) {
}

float SoilMoistureSensor::GetSoilMoisture() const {
  float sum = 0;
  for (size_t i = 0; i < kSampleCount; ++i) {
    sum += ReadPinState();
  }
  return (sum / kSampleCount) * 100;
}

}  // namespace hpa::sensors
