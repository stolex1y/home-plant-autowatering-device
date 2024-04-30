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

String FormatEpochSecondsAsDateTime(const std::chrono::seconds &epoch_sec) {
  const auto time = static_cast<time_t>(epoch_sec.count());
  const auto utc_time = *gmtime(&time);
  char buf[32];
  snprintf(
      buf,
      32,
      "%02d-%02d-%04d %02d:%02d:%02d",
      utc_time.tm_mday,
      utc_time.tm_mon,
      utc_time.tm_year + 1900,
      utc_time.tm_hour,
      utc_time.tm_min,
      utc_time.tm_sec
  );
  return buf;
}

}  // namespace hpa::utils
