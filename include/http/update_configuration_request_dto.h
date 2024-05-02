#ifndef HOME_PLANT_AUTOWATERING_DEVICE_UPDATE_CONFIGURATION_REQUEST_DTO_H
#define HOME_PLANT_AUTOWATERING_DEVICE_UPDATE_CONFIGURATION_REQUEST_DTO_H

#include <Arduino.h>
#include <ArduinoJson.h>

#include "config/common_config.h"

namespace hpa::http {

struct UpdateConfigurationRequestDto {
  String wifi_ssid;
  String wifi_pass;
  String mqtt_username;
  String mqtt_password;
  String mqtt_url;
  String device_id;

  void UpdateCommonConfig(config::CommonConfig &common_config) const;
};

bool convertFromJson(const JsonVariantConst &src, UpdateConfigurationRequestDto &dst);
bool convertToJson(const UpdateConfigurationRequestDto &src, JsonVariant &dst);

}  // namespace hpa::http

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_UPDATE_CONFIGURATION_REQUEST_DTO_H
