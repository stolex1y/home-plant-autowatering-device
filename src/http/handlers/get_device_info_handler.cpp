#include "http/handlers/get_device_info_handler.h"

#include "http/device_info_response_dto.h"
#include "json.h"
#include "utils/wifi/wifi.h"

namespace hpa::http::handlers {

GetDeviceInfoHandler::GetDeviceInfoHandler() {
  http::DeviceInfoResponseDto response{.mac = utils::wifi::ap::GetMacAddress()};
  response_ = ToJsonString(response);
}

bool GetDeviceInfoHandler::canHandle(AsyncWebServerRequest *request) {
  return request->url() == "/device" && request->method() == HTTP_GET;
}

void GetDeviceInfoHandler::handleRequest(AsyncWebServerRequest *request) {
  LOG_INFO("handle get device info request");
  if (!response_) {
    LOG_ERROR("couldn't convert response to json");
    request->send(500, "text/plain", "json conversion error");
    return;
  }
  LOG_DEBUG("send OK response with body: %s", response_->c_str());
  request->send(200, "application/json", response_.value());
}

}  // namespace hpa::http::handlers
