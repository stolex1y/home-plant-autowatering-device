#include "pins/pin.h"

#include <Arduino.h>

#include "logger.h"

namespace hpa::pins {

namespace hardware {

static constexpr const auto kAnalogMaxValue = 1023;

void SetMode(const uint8_t pin_number, const PinMode mode) {
  uint8_t hardware_mode = 0;
  switch (mode) {
    case PinMode::kOutputMode:
      hardware_mode = OUTPUT;
      break;
    case PinMode::kInputMode:
      hardware_mode = INPUT;
      break;
    case PinMode::kInputPullUpMode:
      hardware_mode = INPUT_PULLUP;
      break;
  }
  pinMode(pin_number, hardware_mode);
}

void HardwareWrite(const uint8_t pin_number, const uint16_t value, const PinType pin_type) {
  if (pin_type == PinType::kAnalog) {
    const uint16_t value_constrained = constrain(value, 0, 255);
    analogWrite(pin_number, value_constrained);
  } else {
    uint16_t hardware_value;
    if (value)
      hardware_value = HIGH;
    else
      hardware_value = LOW;
    digitalWrite(pin_number, hardware_value);
  }
}

uint16_t HardwareRead(const uint8_t pin_number, const PinType pin_type) {
  if (pin_type == PinType::kAnalog) {
    return analogRead(pin_number);
  }
  return digitalRead(pin_number);
}

}  // namespace hardware

bool IsInputMode(const PinMode mode) {
  switch (mode) {
    case PinMode::kInputPullUpMode:
    case PinMode::kInputMode:
      return true;
    case PinMode::kOutputMode:
    default:
      return false;
  }
}

bool IsOutputMode(const PinMode mode) {
  return !IsInputMode(mode);
}

String PinTypeToString(const PinType pinType) {
  switch (pinType) {
    case PinType::kAnalog:
      return "analog";
    case PinType::kDigital:
      return "digital";
  }
  return "unknown";
}

Pin::Pin(PinNumber pin_number, PinType type, PinMode mode)
    : pin_number_(pin_number), type_(type), mode_() {
  SetMode(mode);
}

/**
 * @return [0, 1023]: if analog,
 * {0, 1}: if digital,
 * -1: if error pin mode.
 */
std::optional<Pin::PinState> Pin::GetState() const {
  if (IsOutputMode(mode_))
    return std::nullopt;
  return hardware::HardwareRead(pin_number_, type_);
}

/**
 * @param state - [0; 255] - if analog,
 * {0, 1} - if digital.
 *
 * @return false - if pin isn't output pin.
 */
bool Pin::SetState(PinState state) {
  if (IsInputMode(mode_)) {
    LOG_ERROR("tried to set pin state in input mode");
    return false;
  }
  hardware::HardwareWrite(pin_number_, state, type_);
  return true;
}

void Pin::SetMode(PinMode mode) {
  hardware::SetMode(pin_number_, mode);
  mode_ = mode;
}

Pin::PinNumber Pin::GetNumber() const {
  return pin_number_;
}

PinType Pin::GetType() const {
  return type_;
}

namespace analog_values {

const Pin::PinState kMin = 0;
const Pin::PinState kMax = hardware::kAnalogMaxValue;

}  // namespace analog_values

namespace digital_values {

const Pin::PinState kHigh = HIGH;
const Pin::PinState kLow = LOW;

}  // namespace digital_values

}  // namespace hpa::pins
