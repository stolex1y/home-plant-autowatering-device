#ifndef HOME_PLANT_AUTOWATERING_DEVICE_MQTT_CONFIG_H
#define HOME_PLANT_AUTOWATERING_DEVICE_MQTT_CONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>

namespace hpa::config {

constexpr static const auto kDefaultBaseTopic = "autowatering";

struct MqttConfig {
  String url;
  String username;
  String password;
  String base_topic = kDefaultBaseTopic;
};

bool convertToJson(const MqttConfig &src, JsonVariant &dst);
bool convertFromJson(const JsonVariantConst &src, MqttConfig &dst);

bool operator==(const MqttConfig &first, const MqttConfig &second);
bool operator!=(const MqttConfig &first, const MqttConfig &second);

}  // namespace hpa::config

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_MQTT_CONFIG_H
