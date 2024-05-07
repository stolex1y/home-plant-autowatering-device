#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "utils/utils.h"

namespace hpa::config {

constexpr static const uint16_t kDefaultWebServerPort = 8080;

struct HttpConfig {
  uint16_t server_port = kDefaultWebServerPort;
};

bool convertToJson(const HttpConfig &src, JsonVariant &dst);
bool convertFromJson(const JsonVariantConst &src, HttpConfig &dst);

bool operator==(const HttpConfig &first, const HttpConfig &second);
bool operator!=(const HttpConfig &first, const HttpConfig &second);

}  // namespace hpa::config
