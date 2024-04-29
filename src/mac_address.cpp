#include "mac_address.h"

namespace hpa {

bool convertToJson(const MacAddress &src, JsonVariant &dst) {
  String str;
  str = "00-00-00-00-00-00";
  snprintf(
      str.begin(),
      str.length() + 1,
      "%02x-%02x-%02x-%02x-%02x-%02x",
      src.groups[0],
      src.groups[1],
      src.groups[2],
      src.groups[3],
      src.groups[4],
      src.groups[5]
  );
  return dst.set(str);
}

}  // namespace hpa
