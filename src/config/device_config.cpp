#include "config/device_config.h"

#include "json.h"

namespace hpa::config {

bool convertToJson(const DeviceConfig &src, JsonVariant &dst) {
  return SetJsonFromField(dst, "send_period", src.sync_period) &&
         SetJsonFromField(dst, "soil_moisture_min", src.soil_moisture_min) &&
         SetJsonFromField(dst, "soil_moisture_max", src.soil_moisture_max) &&
         SetJsonFromField(dst, "base_topic", src.base_topic);
}

bool convertFromJson(const JsonVariantConst &src, DeviceConfig &dst) {
  return SetFieldFromJson(src, "send_period", dst.sync_period) &&
         SetFieldFromJson(src, "soil_moisture_min", dst.soil_moisture_min) &&
         SetFieldFromJson(src, "soil_moisture_max", dst.soil_moisture_max) &&
         SetFieldFromJson(src, "base_topic", dst.base_topic);
}

bool operator==(const DeviceConfig &first, const DeviceConfig &second) {
  return first.sync_period == second.sync_period &&
         first.soil_moisture_min == second.soil_moisture_min &&
         first.soil_moisture_max == second.soil_moisture_max &&
         first.base_topic == second.base_topic;
}

bool operator!=(const DeviceConfig &first, const DeviceConfig &second) {
  return !(first == second);
}

}  // namespace hpa::config
