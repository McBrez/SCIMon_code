#ifndef WRITE_DEVICE_MESSAGE_HPP
#define WRITE_DEVICE_MESSAGE_HPP

// Standard includes
#include <variant>

// Project includes
#include <device_message.hpp>

using namespace std;
using AdditionalData = variant<bool, int, float, string>;

namespace Messages {

/**
 * @brief Identifies the reasons for the write operation.
 */
enum WriteDeviceTopic {
  /// Invalid topic
  INVALID_TOPIC,
  /// The device shall start its operation with a previously sent configuration.
  RUN_TOPIC,
  /// The device shall stop its operation.
  STOP_TOPIC,
  /// The message contains data for the device.
  DATA_TOPIC
};

/**
 * @brief A message that is meant to trigger certain processes in a device.
 */
class WriteDeviceMessage : public DeviceMessage {
public:
  WriteDeviceMessage(WriteDeviceTopic topic);
  WriteDeviceMessage(WriteDeviceTopic topic, AdditionalData additionalData);

  AdditionalData getAdditionalData();
  WriteDeviceTopic getTopic();

  virtual string serialize() override;

private:
  AdditionalData additionalData;
  WriteDeviceTopic topic;
};
} // namespace Messages

#endif
