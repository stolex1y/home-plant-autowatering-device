#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "config/device_config.h"
#include "config/http_config.h"
#include "config/mqtt_config.h"
#include "config/wifi_config.h"

namespace hpa::config {

struct CommonConfig {
  MqttConfig mqtt_config;
  WifiConfig wifi_config;
  DeviceConfig device_config;
  HttpConfig http_config;
};

bool convertToJson(const CommonConfig &src, JsonVariant &dst);
bool convertFromJson(const JsonVariantConst &src, CommonConfig &dst);

bool operator==(const CommonConfig &first, const CommonConfig &second);
bool operator!=(const CommonConfig &first, const CommonConfig &second);

}  // namespace hpa::config
