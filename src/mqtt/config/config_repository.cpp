#include "mqtt/config/config_repository.h"

#include "json.h"
#include "logger.h"
#include "mqtt/config/config_dto.h"

namespace hpa::mqtt::config {

ConfigRemoteRepository::ConfigRemoteRepository(
    MqttClient &mqtt_client,
    const hpa::config::MqttConfig &mqtt_config,
    hpa::config::ConfigRepository<hpa::config::CommonConfig> &config_repository
)
    : mqtt_client_(mqtt_client),
      config_repository_(config_repository),
      config_topic_(mqtt_config.config_topic) {
  mqtt_client.Subscribe(
      config_topic_,
      2,
      [this](const String &payload, const AsyncMqttClientMessageProperties &properties) {
        HandleMessage(payload);
      }
  );
}

ConfigRemoteRepository::~ConfigRemoteRepository() {
  mqtt_client_.Unsubscribe(config_topic_);
}

void ConfigRemoteRepository::HandleMessage(const String &payload) {
  LOG_TRACE("receive message");

  auto common_config = config_repository_.ReadConfig();
  if (!common_config) {
    LOG_ERROR("couldn't find common config");
    return;
  }
  LOG_DEBUG("erase retained config message");
  mqtt_client_.Publish(config_topic_, 2, true, "");

  DeserializationError error;
  std::optional<ConfigDto> config_dto;
  std::tie(error, config_dto) = FromJsonString<ConfigDto>(payload);
  if (error || !config_dto) {
    LOG_ERROR("couldn't deserialize request body: %s", error.c_str());
    return;
  }
  LOG_TRACE(
      "parsed config dto: %s", ToJsonString(config_dto.value()).value_or("couldn't encode").c_str()
  );

  config_dto->UpdateConfig(common_config.value());
  config_repository_.WriteConfig(common_config.value());
}

}  // namespace hpa::mqtt::config
