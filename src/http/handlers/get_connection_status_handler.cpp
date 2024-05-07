#include "http/handlers/get_connection_status_handler.h"

#include "http/connection_status_response_dto.h"
#include "json.h"
#include "logger.h"
#include "utils/wifi/wifi.h"

namespace hpa::http::handlers {

bool GetConnectionStatusHandler::canHandle(AsyncWebServerRequest *request) {
  return request->method() == HTTP_GET && request->url() == "/connection";
}

void GetConnectionStatusHandler::handleRequest(AsyncWebServerRequest *request) {
  LOG_INFO("handle get connection status request");
  http::ConnectionStatusResponseDto response{};
  response.connected = utils::wifi::sta::IsConnected();
  const auto response_str = ToJsonString(response);
  if (!response_str) {
    LOG_ERROR("couldn't convert response to json");
    request->send(500, "text/plain", "json conversion error");
    return;
  }
  LOG_DEBUG("send OK response: %s", response_str->c_str());
  request->send(200, "application/json", response_str.value());
}

}  // namespace hpa::http::handlers
