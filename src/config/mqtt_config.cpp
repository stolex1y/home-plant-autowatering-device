#include "config/mqtt_config.h"

#include "json.h"

namespace hpa::config {

bool convertToJson(const MqttConfig &src, JsonVariant &dst) {
  return SetJsonFromField(dst, "url", src.url) && SetJsonFromField(dst, "username", src.username) &&
         SetJsonFromField(dst, "password", src.password) &&
         SetJsonFromField(dst, "base_topic", src.base_topic);
}

bool convertFromJson(const JsonVariantConst &src, MqttConfig &dst) {
  return SetFieldFromJson(src, "url", dst.url) && SetFieldFromJson(src, "username", dst.username) &&
         SetFieldFromJson(src, "password", dst.password) &&
         SetFieldFromJson(src, "base_topic", dst.base_topic);
}

bool operator==(const MqttConfig &first, const MqttConfig &second) {
  return first.username == second.username && first.url == second.url &&
         first.password == second.password && first.base_topic == second.base_topic;
}

bool operator!=(const MqttConfig &first, const MqttConfig &second) {
  return !(first == second);
}

}  // namespace hpa::config
