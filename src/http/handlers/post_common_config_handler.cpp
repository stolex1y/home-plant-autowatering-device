#include "http/handlers/post_common_config_handler.h"

#include <config/config_repository.h>

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
  if (!common_config_) {
    LOG_INFO("received bad request - send BAD REQUEST response");
    request->send(400, "text/plain", "invalid config");
    return;
  }

  if (!common_config_repository_.WriteConfig(common_config_.value())) {
    LOG_ERROR("couldn't write config: %s", ToJsonString(common_config_.value()).c_str());
    LOG_TRACE("reset common config in eeprom");
    common_config_.reset();
    common_config_repository_.ResetConfig();
    request->send(500, "text/plait", "couldn't save config");
    return;
  }

  LOG_TRACE(
      "written common config: %s",
      ToJsonString(common_config_repository_.ReadConfig().value()).c_str()
  );
  wifi::sta::TryConnectToWifi(common_config_->wifi_ssid, common_config_->wifi_pass);
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
    common_config_.reset();
  }
  request_body_.concat(reinterpret_cast<char *>(data), len);

  if (index + len == total) {
    LOG_DEBUG("finish handling body: %s", request_body_.c_str());
    LOG_TRACE("try to transform to json");
    DeserializationError error;
    std::tie(error, common_config_) = FromJsonString<config::CommonConfig>(request_body_);
    request_body_ = String("");
    if (error) {
      LOG_INFO("couldn't deserialize request body: %s", error.c_str());
      return;
    }
  }
}

}  // namespace hpa::http::handlers
