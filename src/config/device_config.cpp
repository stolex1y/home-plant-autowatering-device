#include "config/device_config.h"

#include "json.h"

namespace hpa::config {

bool convertToJson(const DeviceConfig &src, JsonVariant &dst) {
  return SetJsonFromField<decltype(src.sync_period)>(dst, "send_period", src.sync_period) &&
         SetJsonFromField<decltype(src.soil_moisture_min)>(
             dst, "soil_moisture_min", src.soil_moisture_min
         ) &&
         SetJsonFromField<decltype(src.soil_moisture_max)>(
             dst, "soil_moisture_max", src.soil_moisture_max
         );
}

bool convertFromJson(const JsonVariantConst &src, DeviceConfig &dst) {
  return SetFieldFromJson<decltype(dst.sync_period)>(src, "send_period", dst.sync_period) &&
         SetFieldFromJson<decltype(dst.soil_moisture_min)>(
             src, "soil_moisture_min", dst.soil_moisture_min
         ) &&
         SetFieldFromJson<decltype(dst.soil_moisture_max)>(
             src, "soil_moisture_max", dst.soil_moisture_max
         );
}

bool operator==(const DeviceConfig &first, const DeviceConfig &second) {
  return first.sync_period == second.sync_period &&
         first.soil_moisture_min == second.soil_moisture_min &&
         first.soil_moisture_max == second.soil_moisture_max;
}

bool operator!=(const DeviceConfig &first, const DeviceConfig &second) {
  return !(first == second);
}

}  // namespace hpa::config
