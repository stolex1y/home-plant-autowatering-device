#ifndef HOME_PLANT_AUTOWATERING_DEVICE_SOIL_MOISTURE_SENSOR_H
#define HOME_PLANT_AUTOWATERING_DEVICE_SOIL_MOISTURE_SENSOR_H

#include "analog_sensor.h"

namespace hpa::sensors {

class SoilMoistureSensor : public AnalogSensor {
 public:
  explicit SoilMoistureSensor(pins::Pin::PinNumber pin_number);

  [[nodiscard]] float GetSoilMoisture() const;

 private:
  static constexpr const auto kSampleCount = 128;
  static constexpr const auto kMin = 300;
  //  static constexpr const auto kMin = 655;
  static constexpr const auto kMax = 720;
  //  static constexpr const auto kMax = 1024;
};

}  // namespace hpa::sensors

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_SOIL_MOISTURE_SENSOR_H
