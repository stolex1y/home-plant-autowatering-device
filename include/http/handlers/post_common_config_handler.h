#ifndef HOME_PLANT_AUTOWATERING_DEVICE_POST_COMMON_CONFIG_HANDLER_H
#define HOME_PLANT_AUTOWATERING_DEVICE_POST_COMMON_CONFIG_HANDLER_H

#include <ESPAsyncWebServer.h>

#include "config/common_config.h"
#include "config/config_repository.h"
#include "http/update_configuration_request_dto.h"

namespace hpa::http::handlers {

class PostCommonConfigHandler : public AsyncWebHandler {
 public:
  explicit PostCommonConfigHandler(
      config::ConfigRepository<config::CommonConfig> &common_config_repository
  );

  bool canHandle(AsyncWebServerRequest *request) override;
  void handleRequest(AsyncWebServerRequest *request) override;
  void handleBody(
      AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total
  ) override;

 private:
  String request_body_;
  std::optional<UpdateConfigurationRequestDto> request_dto_;
  config::ConfigRepository<config::CommonConfig> common_config_repository_;
};

}  // namespace hpa::http::handlers

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_POST_COMMON_CONFIG_HANDLER_H
