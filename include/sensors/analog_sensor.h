#pragma once

#include "pins/pin.h"

namespace hpa::sensors {

class AnalogSensor {
 public:
  AnalogSensor(
      pins::Pin::PinNumber pin_number,
      pins::Pin::PinState min_value = pins::analog_values::kMin,
      pins::Pin::PinState max_value = pins::analog_values::kMax,
      bool inverted = false
  );
  virtual ~AnalogSensor() = default;

 protected:
  const pins::Pin::PinState min_value_;
  const pins::Pin::PinState max_value_;
  const bool inverted_;
  pins::Pin pin_;

  [[nodiscard]] float ReadPinState() const;
};

}  // namespace hpa::sensors
