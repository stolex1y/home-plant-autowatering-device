#pragma once

#include <NTPClient.h>
#include <WiFiUdp.h>

#include <chrono>

namespace hpa::utils {

class NtpClient : private NTPClient {
 public:
  using Duration = std::chrono::seconds;

  explicit NtpClient();

  void Begin();
  void End();
  void Update();
  Duration NowSinceEpoch() const;

 private:
  WiFiUDP ntp_udp_{};
};

}  // namespace hpa::utils
