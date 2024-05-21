#include "mqtt/config/config_dto.h"

#include "json.h"

namespace hpa::mqtt::config {

bool convertToJson(const ConfigDto& src, JsonVariant& dst) {
  return SetJsonFromField(dst, "soilMoistureMin", src.soil_moisture_min) &&
         SetJsonFromField(dst, "pumpTime", src.pump_time) &&
         SetJsonFromField(dst, "pumpDelay", src.pump_delay) &&
         SetJsonFromField(dst, "syncPeriod", src.sync_period);
}

bool convertFromJson(const JsonVariantConst& src, ConfigDto& dst) {
  return SetFieldFromJson(src, "soilMoistureMin", dst.soil_moisture_min) &&
         SetFieldFromJson(src, "pumpTime", dst.pump_time) &&
         SetFieldFromJson(src, "pumpDelay", dst.pump_delay) &&
         SetFieldFromJson(src, "syncPeriod", dst.sync_period);
}

void hpa::mqtt::config::ConfigDto::UpdateConfig(hpa::config::CommonConfig& common_config) const {
  common_config.device_config.sync_period = sync_period;
  common_config.device_config.soil_moisture_min = soil_moisture_min;
  common_config.device_config.pump_delay = pump_delay;
  common_config.device_config.pump_time = pump_time;
}

}  // namespace hpa::mqtt::config
