// Project includes
#include <write_device_message.hpp>

namespace Messages {
WriteDeviceMessage::WriteDeviceMessage(
    shared_ptr<const MessageInterface> source,
    shared_ptr<const MessageInterface> destination, WriteDeviceTopic topic)
    : DeviceMessage(source, destination), topic(topic) {}

WriteDeviceMessage::WriteDeviceMessage(shared_ptr<MessageInterface> source,
                                       shared_ptr<MessageInterface> destination,
                                       WriteDeviceTopic topic,
                                       AdditionalData additionalData)
    : DeviceMessage(source, destination), topic(topic),
      additionalData(additionalData) {}

AdditionalData WriteDeviceMessage::getAdditionalData() {
  return this->additionalData;
}
WriteDeviceTopic WriteDeviceMessage::getTopic() { return this->topic; }

string WriteDeviceMessage::serialize() { return ""; }

} // namespace Messages