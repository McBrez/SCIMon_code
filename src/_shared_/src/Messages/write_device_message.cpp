// Project includes
#include <write_device_message.hpp>

namespace Messages {
WriteDeviceMessage::WriteDeviceMessage(shared_ptr<MessageInterface> source,
                                       shared_ptr<MessageInterface> destination,
                                       WriteDeviceTopic topic)
    : DeviceMessage(source), destination(destination), topic(topic) {}

WriteDeviceMessage::WriteDeviceMessage(shared_ptr<MessageInterface> source,
                                       shared_ptr<MessageInterface> destination,
                                       WriteDeviceTopic topic,
                                       AdditionalData additionalData)
    : DeviceMessage(source), destination(destination), topic(topic),
      additionalData(additionalData) {}

AdditionalData WriteDeviceMessage::getAdditionalData() {
  return this->additionalData;
}
WriteDeviceTopic WriteDeviceMessage::getTopic() { return this->topic; }

string WriteDeviceMessage::serialize() { return ""; }

shared_ptr<const MessageInterface> WriteDeviceMessage::getDestination() {
  return this->destination;
}
} // namespace Messages