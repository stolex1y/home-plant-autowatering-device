#pragma once

#include <ArduinoJson.h>

#include "config/common_config.h"

namespace hpa::mqtt::config {

struct ConfigDto {
  float soil_moisture_min;
  uint64_t pump_time;
  uint64_t pump_delay;
  uint64_t sync_period;

  void UpdateConfig(::hpa::config::CommonConfig &common_config) const;
};

bool convertToJson(const ConfigDto &src, JsonVariant &dst);
bool convertFromJson(const JsonVariantConst &src, ConfigDto &dst);

}  // namespace hpa::mqtt::config
