#include "utils/ntp_client.h"

#include "logger.h"
#include "utils/utils.h"

namespace hpa::utils {

using namespace utils::chrono_literals;

NtpClient::NtpClient() : NTPClient(ntp_udp_) {
  setTimeOffset(0);
  setUpdateInterval(15_min);
}

void NtpClient::Begin() {
  LOG_DEBUG("begin ntp client");
  begin();
  forceUpdate();
}

void NtpClient::Update() {
  update();
}

void NtpClient::End() {
  LOG_DEBUG("ntp client end");
  end();
}

NtpClient::Duration NtpClient::NowSinceEpoch() const {
  return std::chrono::seconds(getEpochTime());
}

}  // namespace hpa::utils
