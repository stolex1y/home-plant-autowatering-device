#include "environment_sensor.h"

#include "logger.h"

namespace hpa::sensors {

EnvironmentSensor::EnvironmentSensor() {
  sensor_.setMode(FORCED_MODE);
  sensor_.setHumOversampling(OVERSAMPLING_16);
  sensor_.setPressOversampling(OVERSAMPLING_16);
  sensor_.setTempOversampling(OVERSAMPLING_16);
  if (!sensor_.begin()) {
    LOG_ERROR("couldn't init sensor");
  }
}

float EnvironmentSensor::GetTemperature() const {
  LOG_DEBUG("get temperature");
  UpdateMeasurements();
  return sensor_.readTemperature();
}

float EnvironmentSensor::GetPressure() const {
  LOG_DEBUG("get pressure");
  UpdateMeasurements();
  return sensor_.readPressure();
}

float EnvironmentSensor::GetHumidity() const {
  LOG_DEBUG("get humidity");
  UpdateMeasurements();
  return sensor_.readHumidity();
}

void EnvironmentSensor::UpdateMeasurements() const {
  LOG_DEBUG("update measurements");
  sensor_.oneMeasurement();
  while (sensor_.isMeasuring()) {
    // just wait
  }
}

}  // namespace hpa::sensors
