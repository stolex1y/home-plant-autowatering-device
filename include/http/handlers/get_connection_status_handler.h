#ifndef HOME_PLANT_AUTOWATERING_DEVICE_GET_CONNECTION_STATUS_HANDLER_H
#define HOME_PLANT_AUTOWATERING_DEVICE_GET_CONNECTION_STATUS_HANDLER_H

#include <ESPAsyncWebServer.h>

namespace hpa::http::handlers {

class GetConnectionStatusHandler : public AsyncWebHandler {
 public:
  bool canHandle(AsyncWebServerRequest *request) override;
  void handleRequest(AsyncWebServerRequest *request) override;
};

}  // namespace hpa::http::handlers

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_GET_CONNECTION_STATUS_HANDLER_H
