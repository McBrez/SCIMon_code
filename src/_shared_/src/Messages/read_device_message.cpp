// Project includes
#include <read_device_message.hpp>

namespace Messages {

ReadDeviceMessage::ReadDeviceMessage(
    UserId source, UserId destination, ReadDeviceTopic topic,
    ReadPayload *readPayloadData,
    std::shared_ptr<WriteDeviceMessage> originalMessage)
    : DeviceMessage(source, destination), topic(topic),
      readPayload(readPayloadData), originalMessage(originalMessage) {}

ReadDeviceMessage::ReadDeviceMessage(
    UserId source, UserId destination, ReadDeviceTopic topic,
    std::shared_ptr<ReadPayload> readPayload,
    std::shared_ptr<WriteDeviceMessage> originalMessage)
    : DeviceMessage(source, destination), topic(topic),
      readPayload(readPayload), originalMessage(originalMessage) {}

std::string ReadDeviceMessage::serialize() { return "Read Device Message"; }

std::shared_ptr<ReadPayload> ReadDeviceMessage::getReadPaylod() {
  return this->readPayload;
}

std::shared_ptr<WriteDeviceMessage> ReadDeviceMessage::getOriginalMessage() {
  return this->originalMessage;
}

ReadDeviceTopic ReadDeviceMessage::getTopic() { return this->topic; }

} // namespace Messages