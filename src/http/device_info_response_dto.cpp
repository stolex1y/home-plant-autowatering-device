#include "http/device_info_response_dto.h"

#include "json.h"

namespace hpa::http {

bool convertToJson(const DeviceInfoResponseDto &src, JsonVariant &dst) {
  return SetJsonFromField<decltype(src.mac)>(dst, "mac", src.mac);
}

}  // namespace hpa::http
