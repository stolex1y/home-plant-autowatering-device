#include "ntp_client.h"

#include "utils.h"

namespace hpa::time {

using namespace utils::chrono_literals;

NtpClient::NtpClient() : NTPClient(ntp_udp_) {
  setTimeOffset(0);
  setUpdateInterval(15_min);
}

void NtpClient::Begin() {
  begin();
  forceUpdate();
}

void NtpClient::Update() {
  update();
}

NtpClient::Duration NtpClient::NowSinceEpoch() {
  return std::chrono::seconds(getEpochTime());
}

}  // namespace hpa::time
