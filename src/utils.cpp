#include "utils.h"

namespace hpa::utils {

String Format(const char *format, ...) {
  const size_t buf_size = 128;
  char buf[buf_size];
  va_list args;
  va_start(args, format);
  vsnprintf(buf, buf_size, format, args);
  va_end(args);
  return {buf};
}

}  // namespace hpa::utils
