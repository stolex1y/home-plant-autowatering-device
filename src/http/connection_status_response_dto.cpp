#include "http/connection_status_response_dto.h"

#include "json.h"

namespace hpa::http {

bool convertToJson(const ConnectionStatusResponseDto &src, JsonVariant &dst) {
  return SetJsonFromField(dst, "connected", src.connected);
}

}  // namespace hpa::http
