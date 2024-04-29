#ifndef HOME_PLANT_AUTOWATERING_DEVICE_POST_SWITCH_MODE_HANDLER_H
#define HOME_PLANT_AUTOWATERING_DEVICE_POST_SWITCH_MODE_HANDLER_H

#include <ESPAsyncWebServer.h>

#include "config/common_config.h"
#include "config/config_repository.h"
#include "task_queue.h"

namespace hpa::http::handlers {

class PostSwitchModeHandler : public AsyncWebHandler {
 public:
  PostSwitchModeHandler(
      tasks::TaskQueue &task_queue,
      const hpa::config::ConfigRepository<config::CommonConfig> &common_config_repository
  );

  bool canHandle(AsyncWebServerRequest *request) override;
  void handleRequest(AsyncWebServerRequest *request) override;

 private:
  tasks::TaskQueue &task_queue_;
  const hpa::config::ConfigRepository<config::CommonConfig> &common_config_repository_;
};

}  // namespace hpa::http::handlers

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_POST_SWITCH_MODE_HANDLER_H
