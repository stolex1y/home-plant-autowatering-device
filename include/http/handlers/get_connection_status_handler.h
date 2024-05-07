#pragma once

#include <ESPAsyncWebServer.h>

namespace hpa::http::handlers {

class GetConnectionStatusHandler : public AsyncWebHandler {
 public:
  bool canHandle(AsyncWebServerRequest *request) override;
  void handleRequest(AsyncWebServerRequest *request) override;
};

}  // namespace hpa::http::handlers
