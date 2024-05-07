#pragma once

#include <INA219_WE.h>

namespace hpa::sensors {

class BatteryChargeLevelSensor {
 public:
  void Init();
  [[nodiscard]] std::optional<float> GetChargeLevel() const;
  void Enable();
  void Disable();

 private:
  static constexpr const uint64_t kMeasuringTimeout = 1000;
  static constexpr const float kMinVoltage = 3.3;
  static constexpr const float kMaxVoltage = 4.0;

  mutable INA219_WE sensor_;
};

}  // namespace hpa::sensors
