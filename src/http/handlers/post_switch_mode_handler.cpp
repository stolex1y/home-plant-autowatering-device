#include "http/handlers/post_switch_mode_handler.h"

#include "logger.h"

namespace hpa::http::handlers {

PostSwitchModeHandler::PostSwitchModeHandler(
    tasks::TaskQueue &task_queue,
    const hpa::config::ConfigRepository<config::CommonConfig> &common_config_repository
)
    : task_queue_(task_queue), common_config_repository_(common_config_repository) {
}

bool PostSwitchModeHandler::canHandle(AsyncWebServerRequest *request) {
  return request->method() == HTTP_POST && request->url() == "/switch-mode";
}

void PostSwitchModeHandler::handleRequest(AsyncWebServerRequest *request) {
  LOG_INFO("handle post switch mode request");
  if (!common_config_repository_.HasConfig()) {
    LOG_INFO("didn't receive config, send BAD REQUEST response");
    request->send(400, "text/plain", "didn't have config");
    return;
  }
  task_queue_.Push([]() {
    LOG_INFO("restarting...");
    delay(500);
    EspClass::restart();
  });
  LOG_DEBUG("send OK response");
  request->send(200);
}

}  // namespace hpa::http::handlers
