#include "utils/utils.h"

#include <IPAddress.h>

#include <algorithm>
#include <charconv>

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

std::pair<String, uint16_t> SeparateUrlToHostAndPort(const String &url) {
  const auto separator = std::find(url.begin(), url.end(), ':');
  if (separator == url.end()) {
    return {url, 0};
  }
  uint16_t port = 0;
  const auto res = std::from_chars(separator + 1, url.end(), port);
  if (res.ec != std::errc()) {
    return {url, 0};
  }
  return {url.substring(0, std::distance(url.begin(), separator)), port};
}

std::optional<IPAddress> IpAddressFromString(const String &ip_str) {
  IPAddress ip;
  if (!ip.fromString(ip_str)) {
    return std::nullopt;
  }
  return ip;
}

size_t StringHash::operator()(const String &str) const {
  size_t h = 5381;
  char c;
  auto it = str.begin();
  while ((c = *it++))
    h = ((h << 5) + h) + c;
  return h;
}

}  // namespace hpa::utils
