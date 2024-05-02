#include "http/update_configuration_request_dto.h"

#include "json.h"

namespace hpa::http {

bool convertFromJson(const JsonVariantConst &src, UpdateConfigurationRequestDto &dst) {
  return SetFieldFromJson(src, "wifiSsid", dst.wifi_ssid) &&
         SetFieldFromJson(src, "wifiPass", dst.wifi_pass) &&
         SetFieldFromJson(src, "mqttUrl", dst.mqtt_url) &&
         SetFieldFromJson(src, "mqttUsername", dst.mqtt_username) &&
         SetFieldFromJson(src, "mqttPassword", dst.mqtt_password) &&
         SetFieldFromJson(src, "deviceId", dst.device_id);
}

bool convertToJson(const UpdateConfigurationRequestDto &src, JsonVariant &dst) {
  return SetJsonFromField(dst, "wifi_ssid", src.wifi_ssid) &&
         SetJsonFromField(dst, "wifi_pass", src.wifi_pass) &&
         SetJsonFromField(dst, "mqtt_url", src.mqtt_url) &&
         SetJsonFromField(dst, "mqtt_username", src.mqtt_username) &&
         SetJsonFromField(dst, "mqtt_password", src.mqtt_password) &&
         SetJsonFromField(dst, "device_id", src.device_id);
}

void UpdateConfigurationRequestDto::UpdateCommonConfig(config::CommonConfig &common_config) const {
  common_config.mqtt_config.url = mqtt_url;
  common_config.mqtt_config.username = mqtt_username;
  common_config.mqtt_config.password = mqtt_password;
  common_config.device_id = device_id;
  common_config.wifi_config.wifi_pass = wifi_pass;
  common_config.wifi_config.wifi_ssid = wifi_ssid;
}

}  // namespace hpa::http
