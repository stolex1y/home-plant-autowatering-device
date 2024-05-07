#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

namespace hpa::config {

constexpr static const auto kDefaultBaseTopic = "autowatering";
constexpr static const auto kDefaultWillTopic = "will";
constexpr static const auto kDefaultConfigTopic = "config";

struct MqttConfig {
  String url;
  String username;
  String password;
  String base_topic = kDefaultBaseTopic;
  String will_topic = kDefaultWillTopic;
  String config_topic = kDefaultConfigTopic;
};

bool convertToJson(const MqttConfig &src, JsonVariant &dst);
bool convertFromJson(const JsonVariantConst &src, MqttConfig &dst);

bool operator==(const MqttConfig &first, const MqttConfig &second);
bool operator!=(const MqttConfig &first, const MqttConfig &second);

}  // namespace hpa::config
