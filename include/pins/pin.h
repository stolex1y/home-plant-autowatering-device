#pragma once

#include <Arduino.h>

namespace hpa::pins {

enum class PinType : uint8_t { kAnalog = 0, kDigital = 1 };

String PinTypeToString(PinType pin_type);

enum class PinMode : uint8_t { kInputMode, kOutputMode, kInputPullUpMode };

[[nodiscard]] bool IsInputMode(PinMode mode);
[[nodiscard]] bool IsOutputMode(PinMode mode);

class Pin {
 public:
  using PinState = uint16_t;
  using PinNumber = uint8_t;

  Pin(PinNumber pin_number, PinType type, PinMode mode);

  [[nodiscard]] std::optional<Pin::PinState> GetState() const;
  bool SetState(PinState state);
  void SetMode(PinMode mode);
  [[nodiscard]] PinNumber GetNumber() const;
  [[nodiscard]] PinType GetType() const;

 private:
  PinNumber pin_number_;
  PinType type_;
  PinMode mode_;
};

namespace analog_values {
extern const Pin::PinState kMin;
extern const Pin::PinState kMax;
}  // namespace analog_values

namespace digital_values {
extern const Pin::PinState kHigh;
extern const Pin::PinState kLow;
}  // namespace digital_values

}  // namespace hpa::pins
