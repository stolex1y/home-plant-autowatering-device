#include "config/common_config.h"

#include "json.h"

namespace hpa::config {

bool convertToJson(const CommonConfig &src, JsonVariant &dst) {
  return SetJsonFromField(dst, "device_id", src.device_id) &&
         SetJsonFromField(dst, "wifi_ssid", src.wifi_ssid) &&
         SetJsonFromField(dst, "wifi_pass", src.wifi_pass) &&
         SetJsonFromField(dst, "mqtt_url", src.mqtt_url) &&
         SetJsonFromField(dst, "mqtt_username", src.mqtt_username) &&
         SetJsonFromField(dst, "mqtt_password", src.mqtt_password);
}

bool convertFromJson(const JsonVariantConst &src, CommonConfig &dst) {
  return SetFieldFromJson(src, "device_id", dst.device_id) &&
         SetFieldFromJson(src, "wifi_ssid", dst.wifi_ssid) &&
         SetFieldFromJson(src, "wifi_pass", dst.wifi_pass) &&
         SetFieldFromJson(src, "mqtt_url", dst.mqtt_url) &&
         SetFieldFromJson(src, "mqtt_username", dst.mqtt_username) &&
         SetFieldFromJson(src, "mqtt_password", dst.mqtt_password);
}

bool operator==(const CommonConfig &first, const CommonConfig &second) {
  return first.wifi_ssid == second.wifi_ssid && first.wifi_pass == second.wifi_pass &&
         first.mqtt_username == second.mqtt_username && first.mqtt_url == second.mqtt_url &&
         first.mqtt_password == second.mqtt_password && first.device_id == second.device_id;
}

bool operator!=(const CommonConfig &first, const CommonConfig &second) {
  return !(first == second);
}

}  // namespace hpa::config
