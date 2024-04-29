#include "config/common_config.h"

#include "json.h"

namespace hpa::config {

bool convertToJson(const CommonConfig &src, JsonVariant &dst) {
  return SetJsonFromField<String>(dst, "device_id", src.device_id) &&
         SetJsonFromField<String>(dst, "wifi_ssid", src.wifi_ssid) &&
         SetJsonFromField<String>(dst, "wifi_pass", src.wifi_pass) &&
         SetJsonFromField<String>(dst, "mqtt_url", src.mqtt_url) &&
         SetJsonFromField<String>(dst, "mqtt_username", src.mqtt_username) &&
         SetJsonFromField<String>(dst, "mqtt_password", src.mqtt_password);
}

bool convertFromJson(const JsonVariantConst &src, CommonConfig &dst) {
  return SetFieldFromJson<String>(src, "device_id", dst.device_id) &&
         SetFieldFromJson<String>(src, "wifi_ssid", dst.wifi_ssid) &&
         SetFieldFromJson<String>(src, "wifi_pass", dst.wifi_pass) &&
         SetFieldFromJson<String>(src, "mqtt_url", dst.mqtt_url) &&
         SetFieldFromJson<String>(src, "mqtt_username", dst.mqtt_username) &&
         SetFieldFromJson<String>(src, "mqtt_password", dst.mqtt_password);
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
