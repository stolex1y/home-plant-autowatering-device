#include "config/device_config.h"

#include "json.h"

namespace hpa::config {

bool convertToJson(const DeviceConfig &src, JsonVariant &dst) {
  return SetJsonFromField(dst, "send_period", src.sync_period) &&
         SetJsonFromField(dst, "soil_moisture_min", src.soil_moisture_min) &&
         SetJsonFromField(dst, "soil_moisture_max", src.soil_moisture_max) &&
         SetJsonFromField(dst, "pump_time", src.pump_time) &&
         SetJsonFromField(dst, "pump_delay", src.pump_delay) &&
         SetJsonFromField(dst, "device_id", src.device_id);
}

bool convertFromJson(const JsonVariantConst &src, DeviceConfig &dst) {
  return SetFieldFromJson(src, "send_period", dst.sync_period) &&
         SetFieldFromJson(src, "soil_moisture_min", dst.soil_moisture_min) &&
         SetFieldFromJson(src, "device_id", dst.device_id) &&
         SetFieldFromJson(src, "pump_time", dst.pump_time) &&
         SetFieldFromJson(src, "pump_delay", dst.pump_delay) &&
         SetFieldFromJson(src, "soil_moisture_max", dst.soil_moisture_max);
}

bool operator==(const DeviceConfig &first, const DeviceConfig &second) {
  return first.sync_period == second.sync_period &&
         first.soil_moisture_min == second.soil_moisture_min &&
         first.soil_moisture_max == second.soil_moisture_max &&
         first.pump_time == second.pump_time && first.pump_delay == second.pump_delay;
}

bool operator!=(const DeviceConfig &first, const DeviceConfig &second) {
  return !(first == second);
}

}  // namespace hpa::config
