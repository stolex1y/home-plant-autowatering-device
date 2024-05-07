#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include <cinttypes>

namespace hpa::utils {

struct MacAddress {
  uint8_t groups[6];
};

bool convertToJson(const MacAddress &src, JsonVariant &dst);

}  // namespace hpa::utils
