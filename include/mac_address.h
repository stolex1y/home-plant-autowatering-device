#ifndef HOME_PLANT_AUTOWATERING_DEVICE_MAC_H
#define HOME_PLANT_AUTOWATERING_DEVICE_MAC_H

#include <Arduino.h>
#include <ArduinoJson.h>

#include <cinttypes>

namespace hpa {

struct MacAddress {
  uint8_t groups[6];
};

bool convertToJson(const MacAddress &src, JsonVariant &dst);

}  // namespace hpa

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_MAC_H
