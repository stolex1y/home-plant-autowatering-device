#include "http/handlers/get_device_info_handler.h"

#include "http/device_info_response_dto.h"
#include "json.h"
#include "wifi.h"

namespace hpa::http::handlers {

GetDeviceInfoHandler::GetDeviceInfoHandler() {
  http::DeviceInfoResponseDto response{.mac = wifi::ap::GetMacAddress()};
  response_ = ToJsonString(response);
}

bool GetDeviceInfoHandler::canHandle(AsyncWebServerRequest *request) {
  return request->url() == "/device" && request->method() == HTTP_GET;
}

void GetDeviceInfoHandler::handleRequest(AsyncWebServerRequest *request) {
  LOG_INFO("handle get device info request");
  LOG_DEBUG("send OK response with body: %s", response_.c_str());
  request->send(200, "application/json", response_);
}

}  // namespace hpa::http::handlers
