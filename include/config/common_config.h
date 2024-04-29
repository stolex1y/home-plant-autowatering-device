#ifndef HOME_PLANT_AUTOWATERING_DEVICE_CONFIG_H
#define HOME_PLANT_AUTOWATERING_DEVICE_CONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>

namespace hpa::config {

struct CommonConfig {
  String device_id;
  String wifi_ssid;
  String wifi_pass;
  String mqtt_url;
  String mqtt_username;
  String mqtt_password;
};

bool convertToJson(const CommonConfig &src, JsonVariant &dst);
bool convertFromJson(const JsonVariantConst &src, CommonConfig &dst);

bool operator==(const CommonConfig &first, const CommonConfig &second);
bool operator!=(const CommonConfig &first, const CommonConfig &second);

}  // namespace hpa::config

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_CONFIG_H
