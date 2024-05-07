#include "config/mqtt_config.h"

#include "json.h"

namespace hpa::config {

bool convertToJson(const MqttConfig &src, JsonVariant &dst) {
  return SetJsonFromField(dst, "url", src.url) && SetJsonFromField(dst, "username", src.username) &&
         SetJsonFromField(dst, "password", src.password) &&
         SetJsonFromField(dst, "will_topic", src.will_topic) &&
         SetJsonFromField(dst, "config_topic", src.config_topic) &&
         SetJsonFromField(dst, "base_topic", src.base_topic);
}

bool convertFromJson(const JsonVariantConst &src, MqttConfig &dst) {
  return SetFieldFromJson(src, "url", dst.url) && SetFieldFromJson(src, "username", dst.username) &&
         SetFieldFromJson(src, "password", dst.password) &&
         SetFieldFromJson(src, "will_topic", dst.will_topic) &&
         SetFieldFromJson(src, "config_topic", dst.config_topic) &&
         SetFieldFromJson(src, "base_topic", dst.base_topic);
}

bool operator==(const MqttConfig &first, const MqttConfig &second) {
  return first.username == second.username && first.url == second.url &&
         first.password == second.password && first.base_topic == second.base_topic &&
         first.will_topic == second.will_topic && first.config_topic == second.config_topic;
}

bool operator!=(const MqttConfig &first, const MqttConfig &second) {
  return !(first == second);
}

}  // namespace hpa::config
