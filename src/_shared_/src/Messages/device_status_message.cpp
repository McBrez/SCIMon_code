#include <device_status_message.hpp>

namespace Messages {

DeviceStatusMessage::DeviceStatusMessage(
    shared_ptr<MessageInterface> source, ReadDeviceTopic topic,
    ReadPayload *readPayload, shared_ptr<WriteDeviceMessage> originalMessage,
    DeviceStatus deviceStatus, string additionalInformation)
    : ReadDeviceMessage(source, topic, readPayload, originalMessage),
      deviceStatus(deviceStatus) {}

DeviceStatus DeviceStatusMessage::getDeviceStatus() {
  return this->deviceStatus;
}
} // namespace Messages