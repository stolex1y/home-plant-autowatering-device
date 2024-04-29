#ifndef HOME_PLANT_AUTOWATERING_DEVICE_DEVICE_CONFIG_H
#define HOME_PLANT_AUTOWATERING_DEVICE_DEVICE_CONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>

namespace hpa::config {

struct DeviceConfig {
  uint64_t sync_period;
  int16_t soil_moisture_min;
  int16_t soil_moisture_max;
};

bool convertToJson(const DeviceConfig &src, JsonVariant &dst);
bool convertFromJson(const JsonVariantConst &src, DeviceConfig &dst);

bool operator==(const DeviceConfig &first, const DeviceConfig &second);
bool operator!=(const DeviceConfig &first, const DeviceConfig &second);

}  // namespace hpa::config

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_DEVICE_CONFIG_H
