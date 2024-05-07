#pragma once

#include "config/common_config.h"
#include "config/config_repository.h"
#include "config/mqtt_config.h"
#include "mqtt/mqtt_client.h"

namespace hpa::mqtt::config {

class ConfigRemoteRepository {
 public:
  ConfigRemoteRepository(
      MqttClient &mqtt_client,
      const hpa::config::MqttConfig &mqtt_config,
      hpa::config::ConfigRepository<hpa::config::CommonConfig> &config_repository
  );
  ~ConfigRemoteRepository();

 private:
  MqttClient &mqtt_client_;
  hpa::config::ConfigRepository<hpa::config::CommonConfig> &config_repository_;
  String config_topic_;

  void HandleMessage(const String &payload);
};

}  // namespace hpa::mqtt::config
