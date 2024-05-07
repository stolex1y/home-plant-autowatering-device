#pragma once

#include <BH1750.h>

namespace hpa::sensors {

class LightLevelSensor {
 public:
  void Init();
  [[nodiscard]] std::optional<float> GetLightLevelLx() const;

 private:
  constexpr static const uint64_t kMeasuringTimeout = 1000;

  mutable BH1750 sensor_;
};

}  // namespace hpa::sensors
