// Project includes
#include <write_device_message.hpp>

namespace Messages {
WriteDeviceMessage::WriteDeviceMessage(UserId source, UserId destination,
                                       WriteDeviceTopic topic)
    : DeviceMessage(source, destination), topic(topic) {}

WriteDeviceMessage::WriteDeviceMessage(UserId source, UserId destination,
                                       WriteDeviceTopic topic,
                                       AdditionalData additionalData)
    : DeviceMessage(source, destination), topic(topic),
      additionalData(additionalData) {}

AdditionalData WriteDeviceMessage::getAdditionalData() {
  return this->additionalData;
}
WriteDeviceTopic WriteDeviceMessage::getTopic() { return this->topic; }

string WriteDeviceMessage::serialize() { return "Write Message"; }

} // namespace Messages