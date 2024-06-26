#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <IPAddress.h>

#include <chrono>

namespace hpa::utils {

String Format(const char *format, ...);

template <typename Period>
bool convertToJson(const std::chrono::duration<int64_t, Period> &src, JsonVariant &dst) {
  return dst.set(std::chrono::round<std::chrono::milliseconds>(src).count());
}

template <typename Period>
bool convertFromJson(const JsonVariantConst &src, std::chrono::duration<int64_t, Period> &dst) {
  if (src.isNull()) {
    return false;
  }
  const auto millis_val = src.as<int64_t>();
  const std::chrono::milliseconds millis_dur{millis_val};
  dst = std::chrono::round<std::chrono::duration<int64_t, Period>>(millis_dur);
  return true;
}

String FormatEpochSecondsAsDateTime(const std::chrono::seconds &epoch_sec);

std::pair<String, uint16_t> SeparateUrlToHostAndPort(const String &url);

std::optional<IPAddress> IpAddressFromString(const String &ip_str);

struct StringHash {
  size_t operator()(const String &str) const;
};

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
