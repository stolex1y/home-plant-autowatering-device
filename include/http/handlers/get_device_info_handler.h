#ifndef HOME_PLANT_AUTOWATERING_DEVICE_GET_DEVICE_INFO_HANDLER_H
#define HOME_PLANT_AUTOWATERING_DEVICE_GET_DEVICE_INFO_HANDLER_H

#include <ESPAsyncWebServer.h>

namespace hpa::http::handlers {

class GetDeviceInfoHandler : public AsyncWebHandler {
 public:
  explicit GetDeviceInfoHandler();

  bool canHandle(AsyncWebServerRequest *request) override;
  void handleRequest(AsyncWebServerRequest *request) override;

 private:
  String response_{};
};

}  // namespace hpa::http::handlers

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_GET_DEVICE_INFO_HANDLER_H
