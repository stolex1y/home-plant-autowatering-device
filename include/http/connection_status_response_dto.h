#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

namespace hpa::http {

struct ConnectionStatusResponseDto {
  bool connected;
};

bool convertToJson(const ConnectionStatusResponseDto &src, JsonVariant &dst);

}  // namespace hpa::http
