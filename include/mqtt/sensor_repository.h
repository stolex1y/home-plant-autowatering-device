#ifndef HOME_PLANT_AUTOWATERING_DEVICE_SENSOR_REPOSITORY_H
#define HOME_PLANT_AUTOWATERING_DEVICE_SENSOR_REPOSITORY_H

#include <Arduino.h>

#include "json.h"
#include "mqtt_client.h"

namespace hpa::mqtt {

template <typename ReadingType>
class SensorRepository {
 public:
  void SendReading(ReadingType reading);

 private:
  String topic_;
  MqttClient mqtt_client_;
};

template <typename ReadingType>
void SensorRepository<ReadingType>::SendReading(ReadingType reading) {
  mqtt_client_.Publish(topic_, 2, true, String(reading));
}

}  // namespace hpa::mqtt

#endif  // HOME_PLANT_AUTOWATERING_DEVICE_SENSOR_REPOSITORY_H
