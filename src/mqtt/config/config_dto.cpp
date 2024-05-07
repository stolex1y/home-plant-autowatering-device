#include "mqtt/config/config_dto.h"

#include "json.h"

namespace hpa::mqtt::config {

bool convertToJson(const ConfigDto& src, JsonVariant& dst) {
  return SetJsonFromField(dst, "soil_moisture_min", src.soil_moisture_min) &&
         SetJsonFromField(dst, "soil_moisture_max", src.soil_moisture_max) &&
         SetJsonFromField(dst, "sync_period", src.sync_period);
}

bool convertFromJson(const JsonVariantConst& src, ConfigDto& dst) {
  return SetFieldFromJson(src, "soil_moisture_min", dst.soil_moisture_min) &&
         SetFieldFromJson(src, "soil_moisture_max", dst.soil_moisture_max) &&
         SetFieldFromJson(src, "sync_period", dst.sync_period);
}

void hpa::mqtt::config::ConfigDto::UpdateConfig(hpa::config::CommonConfig& common_config) const {
  common_config.device_config.sync_period = sync_period;
  common_config.device_config.soil_moisture_max = static_cast<int16_t>(soil_moisture_max);
  common_config.device_config.soil_moisture_min = static_cast<int16_t>(soil_moisture_min);
}

}  // namespace hpa::mqtt::config
