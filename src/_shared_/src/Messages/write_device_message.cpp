// Project includes
#include <write_device_message.hpp>

namespace Messages {
WriteDeviceMessage::WriteDeviceMessage(WriteDeviceTopic topic) : topic(topic) {}

WriteDeviceMessage::WriteDeviceMessage(WriteDeviceTopic topic,
                                       AdditionalData additionalData)
    : topic(topic), additionalData(additionalData) {}

AdditionalData WriteDeviceMessage::getAdditionalData() {
  return this->additionalData;
}
WriteDeviceTopic WriteDeviceMessage::getTopic() { return this->topic; }

string WriteDeviceMessage::serialize() { return ""; }
} // namespace Messages