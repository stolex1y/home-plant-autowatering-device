#include "http/connection_status_response_dto.h"

#include "json.h"

namespace hpa::http {

bool convertToJson(const ConnectionStatusResponseDto &src, JsonVariant &dst) {
  return SetJsonFromField<decltype(src.connected)>(dst, "connected", src.connected);
}

}  // namespace hpa::http
