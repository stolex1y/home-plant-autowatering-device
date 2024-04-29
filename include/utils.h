#ifndef HOME_PLANT_AUTOWATERING_DEVICE_UTILS_H
#define HOME_PLANT_AUTOWATERING_DEVICE_UTILS_H

#include <Arduino.h>

namespace hpa::utils {

String Format(const char *format, ...);

inline namespace chrono_literals {

constexpr uint64_t operator""_h(uint64_t hours) {
  return static_cast<uint64_t>(hours * 3600 * 1000);
}

constexpr uint64_t operator""_h(long double hours) {
  return static_cast<uint64_t>(round(hours * 3600 * 1000));
}

constexpr uint64_t operator""_days(uint64_t days) {
  return static_cast<uint64_t>(days * 24 * 3600 * 1000);
}

constexpr uint64_t operator""_days(long double days) {
  return static_cast<uint64_t>(round(days * 24 * 3600 * 1000));
}

constexpr uint64_t operator""_min(long double min) {
  return static_cast<uint64_t>(round(min * 60 * 1000));
}

constexpr uint64_t operator""_min(uint64_t min) {
  return static_cast<uint64_t>(min * 60 * 1000);
}

constexpr uint64_t operator""_s(long double s) {
  return static_cast<uint64_t>(round(s * 1000));
}

constexpr uint64_t operator""_s(uint64_t s) {
  return static_cast<uint64_t>(s * 1000);
}

}  // namespace chrono_literals

}  // namespace hpa::utils

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_UTILS_H
