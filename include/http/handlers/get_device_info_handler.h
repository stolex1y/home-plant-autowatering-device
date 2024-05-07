#pragma once

#include <ESPAsyncWebServer.h>

namespace hpa::http::handlers {

class GetDeviceInfoHandler : public AsyncWebHandler {
 public:
  explicit GetDeviceInfoHandler();

  bool canHandle(AsyncWebServerRequest *request) override;
  void handleRequest(AsyncWebServerRequest *request) override;

 private:
  std::optional<String> response_;
};

}  // namespace hpa::http::handlers
