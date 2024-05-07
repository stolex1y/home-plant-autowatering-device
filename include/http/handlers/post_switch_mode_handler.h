#pragma once

#include <ESPAsyncWebServer.h>

#include "config/common_config.h"
#include "config/config_repository.h"
#include "utils/task_queue.h"

namespace hpa::http::handlers {

class PostSwitchModeHandler : public AsyncWebHandler {
 public:
  PostSwitchModeHandler(
      utils::TaskQueue &task_queue,
      const hpa::config::ConfigRepository<config::CommonConfig> &common_config_repository
  );

  bool canHandle(AsyncWebServerRequest *request) override;
  void handleRequest(AsyncWebServerRequest *request) override;

 private:
  utils::TaskQueue &task_queue_;
  const hpa::config::ConfigRepository<config::CommonConfig> &common_config_repository_;
};

}  // namespace hpa::http::handlers
