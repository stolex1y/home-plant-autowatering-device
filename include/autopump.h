#pragma once

#include "mqtt/state/pump_state_repository.h"
#include "pins/pin.h"
#include "sensors/soil_moisture_sensor.h"
#include "utils/ntp_client.h"

namespace hpa {

class Autopump {
 public:
  Autopump(
      pins::Pin::PinNumber pin_number,
      mqtt::state::PumpStateRepository &pump_state_repository,
      const config::DeviceConfig &device_config,
      const sensors::SoilMoistureSensor &soil_moisture_sensor,
      const utils::NtpClient &ntp_client
  );

  void Enable();

 private:
  pins::Pin pin_;
  mqtt::state::PumpStateRepository &pump_state_repository_;
  const config::DeviceConfig &device_config_;
  const sensors::SoilMoistureSensor &soil_moisture_sensor_;
  const utils::NtpClient &ntp_client_;

  void EnablePump();
  void DisablePump();
};

}  // namespace hpa
