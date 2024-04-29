#include "http/handlers/get_connection_status_handler.h"

#include "http/connection_status_response_dto.h"
#include "json.h"
#include "logger.h"
#include "wifi.h"

namespace hpa::http::handlers {

bool GetConnectionStatusHandler::canHandle(AsyncWebServerRequest *request) {
  return request->method() == HTTP_GET && request->url() == "/connection";
}

void GetConnectionStatusHandler::handleRequest(AsyncWebServerRequest *request) {
  LOG_INFO("handle get connection status request");
  http::ConnectionStatusResponseDto response{};
  response.connected = wifi::sta::IsConnected();
  String response_str = ToJsonString(response);
  LOG_DEBUG("send OK response: %s", response_str.c_str());
  request->send(200, "application/json", response_str);
}

}  // namespace hpa::http::handlers
