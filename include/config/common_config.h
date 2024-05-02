#ifndef HOME_PLANT_AUTOWATERING_DEVICE_CONFIG_H
#define HOME_PLANT_AUTOWATERING_DEVICE_CONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>

#include "config/mqtt_config.h"
#include "config/wifi_config.h"

namespace hpa::config {

struct CommonConfig {
  MqttConfig mqtt_config;
  WifiConfig wifi_config;
  String device_id;
};

bool convertToJson(const CommonConfig &src, JsonVariant &dst);
bool convertFromJson(const JsonVariantConst &src, CommonConfig &dst);

bool operator==(const CommonConfig &first, const CommonConfig &second);
bool operator!=(const CommonConfig &first, const CommonConfig &second);

}  // namespace hpa::config

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_CONFIG_H
