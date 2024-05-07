#include "sensors/light_level_sensor.h"

#include "logger.h"

namespace hpa::sensors {

std::optional<float> LightLevelSensor::GetLightLevelLx() const {
  LOG_DEBUG("get light level");
  const auto start = millis();
  sensor_.configure(BH1750::ONE_TIME_HIGH_RES_MODE_2);
  while (!sensor_.measurementReady(true) && (millis() - start) < kMeasuringTimeout) {
    yield();
  }
  const auto end = millis();
  LOG_TRACE("measuring time: %lu ms", end - start);

  if (sensor_.measurementReady(false)) {
    LOG_ERROR("couldn't get reading");
    return std::nullopt;
  }

  const auto reading = sensor_.readLightLevel();
  if (reading == -1) {
    LOG_ERROR("invalid reading");
    return std::nullopt;
  }
  if (reading == -2) {
    LOG_ERROR("sensor isn't configured");
    return std::nullopt;
  }
  return reading;
}

void LightLevelSensor::Init() {
  if (!sensor_.begin(BH1750::ONE_TIME_HIGH_RES_MODE_2)) {
    LOG_ERROR("couldn't init sensor");
  }
}

}  // namespace hpa::sensors
