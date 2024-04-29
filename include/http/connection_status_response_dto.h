#ifndef HOME_PLANT_AUTOWATERING_DEVICE_CONNECTION_STATUS_RESPONSE_DTO_H
#define HOME_PLANT_AUTOWATERING_DEVICE_CONNECTION_STATUS_RESPONSE_DTO_H

#include <Arduino.h>
#include <ArduinoJson.h>

namespace hpa::http {

struct ConnectionStatusResponseDto {
  bool connected;
};

bool convertToJson(const ConnectionStatusResponseDto &src, JsonVariant &dst);

}  // namespace hpa::http

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_CONNECTION_STATUS_RESPONSE_DTO_H
