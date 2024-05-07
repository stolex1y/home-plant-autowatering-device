#include "sensors/analog_sensor.h"

#include "logger.h"

namespace hpa::sensors {

AnalogSensor::AnalogSensor(
    pins::Pin::PinNumber pin_number,
    pins::Pin::PinState min_value,
    pins::Pin::PinState max_value,
    bool inverted
)
    : min_value_(min_value),
      max_value_(max_value),
      inverted_(inverted),
      pin_(pin_number, pins::PinType::kAnalog, pins::PinMode::kInputMode) {
}

float AnalogSensor::ReadPinState() const {
  const auto state_opt = pin_.GetState();
  if (!state_opt) {
    LOG_ERROR("tried to read state from invalid pin");
    return 0;
  }
  //    LOG_TRACE("pin state raw: %d", state_opt.value());
  const auto state_value = constrain(state_opt.value(), min_value_, max_value_) -
                           min_value_;  // now state in [0, max - min]

  //  LOG_TRACE("pin state: %d", state_value);
  const auto state_max = max_value_ - min_value_;
  const auto state_frac = static_cast<float>(state_value) / static_cast<float>(state_max);
  //  LOG_TRACE("pin state: %.02f", state_frac);
  if (inverted_) {
    return 1 - state_frac;
  }
  return state_frac;
}

}  // namespace hpa::sensors
