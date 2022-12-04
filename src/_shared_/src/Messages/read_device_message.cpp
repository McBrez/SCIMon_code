// Project includes
#include <read_device_message.hpp>

namespace Messages {

ReadDeviceMessage::ReadDeviceMessage(ReadDeviceTopic topic,
                                     ReadPayload *readPayloadData)
    : topic(topic), readPayload(readPayloadData) {}

string ReadDeviceMessage::serialize() { return ""; }

shared_ptr<ReadPayload> ReadDeviceMessage::getReadPaylod() {
  return this->readPayload;
}

} // namespace Messages