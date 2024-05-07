#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "utils/mac_address.h"

namespace hpa::http {

struct DeviceInfoResponseDto {
  utils::MacAddress mac;
};

bool convertToJson(const DeviceInfoResponseDto &src, JsonVariant &dst);

}  // namespace hpa::http
