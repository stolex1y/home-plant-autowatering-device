#ifndef HOME_PLANT_AUTOWATERING_DEVICE_DEVICE_INFO_RESPONSE_DTO_H
#define HOME_PLANT_AUTOWATERING_DEVICE_DEVICE_INFO_RESPONSE_DTO_H

#include <Arduino.h>
#include <ArduinoJson.h>

#include "mac_address.h"

namespace hpa::http {

struct DeviceInfoResponseDto {
  MacAddress mac;
};

bool convertToJson(const DeviceInfoResponseDto &src, JsonVariant &dst);

}  // namespace hpa::http

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_DEVICE_INFO_RESPONSE_DTO_H
