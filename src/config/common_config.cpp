#include "config/common_config.h"

#include "json.h"

namespace hpa::config {

bool convertToJson(const CommonConfig &src, JsonVariant &dst) {
  return SetJsonFromField(dst, "wifi", src.wifi_config) &&
         SetJsonFromField(dst, "device_id", src.device_id) &&
         SetJsonFromField(dst, "mqtt", src.mqtt_config);
}

bool convertFromJson(const JsonVariantConst &src, CommonConfig &dst) {
  return SetFieldFromJson(src, "wifi", dst.wifi_config) &&
         SetFieldFromJson(src, "device_id", dst.device_id) &&
         SetFieldFromJson(src, "mqtt", dst.mqtt_config);
}

bool operator==(const CommonConfig &first, const CommonConfig &second) {
  return first.wifi_config == second.wifi_config && first.mqtt_config == second.mqtt_config &&
         first.device_id == second.device_id;
}

bool operator!=(const CommonConfig &first, const CommonConfig &second) {
  return !(first == second);
}

}  // namespace hpa::config
