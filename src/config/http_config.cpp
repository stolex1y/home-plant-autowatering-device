#include "../../include/config/http_config.h"

#include "json.h"

namespace hpa::config {

bool convertToJson(const HttpConfig& src, JsonVariant& dst) {
  return SetJsonFromField(dst, "server_port", src.server_port);
}

bool convertFromJson(const JsonVariantConst& src, HttpConfig& dst) {
  return SetFieldFromJson(src, "server_port", dst.server_port);
}

bool operator==(const HttpConfig& first, const HttpConfig& second) {
  return first.server_port == second.server_port;
}

bool operator!=(const HttpConfig& first, const HttpConfig& second) {
  return !(first == second);
}

}  // namespace hpa::config
