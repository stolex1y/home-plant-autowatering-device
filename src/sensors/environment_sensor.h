#pragma once

#include <GyverBME280.h>

namespace hpa::sensors {

class EnvironmentSensor {
 public:
  EnvironmentSensor();
  [[nodiscard]] float GetTemperature() const;
  [[nodiscard]] float GetPressure() const;
  [[nodiscard]] float GetHumidity() const;

 private:
  mutable GyverBME280 sensor_;

  void UpdateMeasurements() const;
};

}  // namespace hpa::sensors
