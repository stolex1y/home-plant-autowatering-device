#include "http/handlers/post_common_config_handler.h"

#include "config/config_repository.h"
#include "json.h"
#include "logger.h"
#include "wifi.h"

namespace hpa::http::handlers {

PostCommonConfigHandler::PostCommonConfigHandler(
    config::ConfigRepository<config::CommonConfig> &common_config_repository
)
    : common_config_repository_(common_config_repository) {
}

bool PostCommonConfigHandler::canHandle(AsyncWebServerRequest *request) {
  return request->url() == "/config" && request->method() == HTTP_POST &&
         request->contentType() == "application/json";
}

void PostCommonConfigHandler::handleRequest(AsyncWebServerRequest *request) {
  LOG_INFO("handle post common config request");
  if (!request_dto_) {
    LOG_INFO("received bad request - send BAD REQUEST response");
    request->send(400, "text/plain", "invalid config");
    return;
  }

  auto common_config = common_config_repository_.ReadConfig().value_or(config::CommonConfig{});
  request_dto_->UpdateCommonConfig(common_config);

  if (!common_config_repository_.WriteConfig(common_config)) {
    LOG_ERROR(
        "couldn't write config: %s", ToJsonString(common_config).value_or("couldn't parse").c_str()
    );
    LOG_TRACE("reset common config in eeprom");
    request_dto_.reset();
    common_config_repository_.ResetConfig();
    request->send(500, "text/plain", "couldn't save config");
    return;
  }
  LOG_TRACE("wrote common config successfully");

  wifi::sta::TryConnectToWifi(request_dto_->wifi_ssid, request_dto_->wifi_pass);
  request->send(200);
  LOG_DEBUG("send OK response");
}

void PostCommonConfigHandler::handleBody(
    AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total
) {
  if (!index) {
    LOG_DEBUG("start handling body of post common config request");
    LOG_TRACE("reserve %zu bytes to message body", total);
    request_body_.reserve(total + 1);
    request_dto_.reset();
  }
  request_body_.concat(reinterpret_cast<char *>(data), len);

  if (index + len == total) {
    LOG_DEBUG("finish handling body: %s", request_body_.c_str());
    LOG_TRACE("try to transform to json");
    DeserializationError error;
    std::tie(error, request_dto_) = FromJsonString<UpdateConfigurationRequestDto>(request_body_);
    request_body_ = String();
    if (error) {
      LOG_INFO("couldn't deserialize request body: %s", error.c_str());
      return;
    }
    LOG_TRACE(
        "parsed request dto: %s",
        ToJsonString(request_dto_.value()).value_or("couldn't parse").c_str()
    );
  }
}

}  // namespace hpa::http::handlers
