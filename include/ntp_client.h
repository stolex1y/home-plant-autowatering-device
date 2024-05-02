#ifndef HOME_PLANT_AUTOWATERING_DEVICE_NTP_CLIENT_H
#define HOME_PLANT_AUTOWATERING_DEVICE_NTP_CLIENT_H

#include <NTPClient.h>
#include <WiFiUdp.h>

#include <chrono>

namespace hpa::time {

class NtpClient : private NTPClient {
 public:
  using Duration = std::chrono::seconds;

  explicit NtpClient();

  void Begin();
  void End();
  void Update();
  Duration NowSinceEpoch();

 private:
  WiFiUDP ntp_udp_{};
};

}  // namespace hpa::time

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_NTP_CLIENT_H
