#include <device_status_message.hpp>

namespace Messages {

DeviceStatusMessage::DeviceStatusMessage(
    shared_ptr<const MessageInterface> source,
    shared_ptr<const MessageInterface> destination, ReadDeviceTopic topic,
    ReadPayload *readPayload, shared_ptr<WriteDeviceMessage> originalMessage,
    DeviceStatus deviceStatus, string additionalInformation)
    : ReadDeviceMessage(source, destination, topic, readPayload,
                        originalMessage),
      deviceStatus(deviceStatus) {}

DeviceStatus DeviceStatusMessage::getDeviceStatus() {
  return this->deviceStatus;
}
} // namespace Messages