#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "utils/utils.h"

namespace hpa::config {

using namespace utils::chrono_literals;

constexpr static const uint64_t kDefaultSyncPeriod = 20_s;
constexpr static const int16_t kDefaultSoilMoistureMin = 0;
constexpr static const uint64_t kDefaultPumpTime = 10_s;
constexpr static const uint64_t kDefaultPumpDelay = 1_days;

struct DeviceConfig {
  String device_id;
  uint64_t sync_period = kDefaultSyncPeriod;
  float soil_moisture_min = kDefaultSoilMoistureMin;
  uint64_t pump_time = kDefaultPumpTime;
  uint64_t pump_delay = kDefaultPumpDelay;
};

bool convertToJson(const DeviceConfig &src, JsonVariant &dst);
bool convertFromJson(const JsonVariantConst &src, DeviceConfig &dst);

bool operator==(const DeviceConfig &first, const DeviceConfig &second);
bool operator!=(const DeviceConfig &first, const DeviceConfig &second);

}  // namespace hpa::config
