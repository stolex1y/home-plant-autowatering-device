#ifndef HOME_PLANT_AUTOWATERING_DEVICE_DEVICE_CONFIG_H
#define HOME_PLANT_AUTOWATERING_DEVICE_DEVICE_CONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>

#include "utils.h"

namespace hpa::config {

constexpr static const uint64_t kDefaultSyncPeriod = 20000;
constexpr static const int16_t kDefaultSoilMoistureMin = INT16_MIN;
constexpr static const int16_t kDefaultSoilMoistureMax = INT16_MAX;

struct DeviceConfig {
  uint64_t sync_period = kDefaultSyncPeriod;
  int16_t soil_moisture_min = kDefaultSoilMoistureMin;
  int16_t soil_moisture_max = kDefaultSoilMoistureMax;
};

bool convertToJson(const DeviceConfig &src, JsonVariant &dst);
bool convertFromJson(const JsonVariantConst &src, DeviceConfig &dst);

bool operator==(const DeviceConfig &first, const DeviceConfig &second);
bool operator!=(const DeviceConfig &first, const DeviceConfig &second);

}  // namespace hpa::config

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_DEVICE_CONFIG_H
