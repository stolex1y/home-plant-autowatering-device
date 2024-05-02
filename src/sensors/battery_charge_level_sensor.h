#pragma once

#include <INA219_WE.h>

namespace hpa::sensors {

class BatteryChargeLevelSensor {
 public:
  void Init();
  void Enable();
  void Disable();
  [[nodiscard]] float GetChargeLevel() const;

 private:
  static constexpr const float kMinVoltage = 2.5;
  static constexpr const float kMaxVoltage = 3.7;

  mutable INA219_WE sensor_;
};

}  // namespace hpa::sensors
