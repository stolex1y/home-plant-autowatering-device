#pragma once

#include <GyverBME280.h>

namespace hpa::sensors {

class EnvironmentSensor {
 public:
  explicit EnvironmentSensor();
  [[nodiscard]] std::optional<float> GetTemperature() const;
  [[nodiscard]] std::optional<float> GetPressure() const;
  [[nodiscard]] std::optional<float> GetHumidity() const;

 private:
  mutable GyverBME280 sensor_;

  void UpdateMeasurements() const;
};

}  // namespace hpa::sensors
