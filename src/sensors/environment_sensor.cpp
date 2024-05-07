#include "sensors/environment_sensor.h"

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

std::optional<float> EnvironmentSensor::GetTemperature() const {
  LOG_DEBUG("get temperature");
  UpdateMeasurements();
  const auto reading = sensor_.readTemperature();
  if (reading == 0) {
    return std::nullopt;
  }
  return reading;
}

std::optional<float> EnvironmentSensor::GetPressure() const {
  LOG_DEBUG("get pressure");
  UpdateMeasurements();
  const auto reading = sensor_.readPressure();
  if (reading == 0) {
    return std::nullopt;
  }
  return reading;
}

std::optional<float> EnvironmentSensor::GetHumidity() const {
  LOG_DEBUG("get humidity");
  UpdateMeasurements();
  const auto reading = sensor_.readHumidity();
  if (reading == 0) {
    return std::nullopt;
  }
  return reading;
}

void EnvironmentSensor::UpdateMeasurements() const {
  LOG_DEBUG("update measurements");
  sensor_.oneMeasurement();
  while (sensor_.isMeasuring()) {
    // just wait
  }
}

}  // namespace hpa::sensors
