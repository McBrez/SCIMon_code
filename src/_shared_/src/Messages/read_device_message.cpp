// Project includes
#include <read_device_message.hpp>

namespace Messages {

ReadDeviceMessage::ReadDeviceMessage(
    UserId source, UserId destination, ReadDeviceTopic topic,
    ReadPayload *readPayloadData,
    shared_ptr<WriteDeviceMessage> originalMessage)
    : DeviceMessage(source, destination), topic(topic),
      readPayload(readPayloadData), originalMessage(originalMessage) {}

string ReadDeviceMessage::serialize() { return ""; }

shared_ptr<ReadPayload> ReadDeviceMessage::getReadPaylod() {
  return this->readPayload;
}

shared_ptr<WriteDeviceMessage> ReadDeviceMessage::getOriginalMessage() {
  return this->originalMessage;
}

ReadDeviceTopic ReadDeviceMessage::getTopic() { return this->topic; }

} // namespace Messages