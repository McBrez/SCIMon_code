// Project include
#include <init_device_message.hpp>

namespace Messages {
InitDeviceMessage::InitDeviceMessage(
    shared_ptr<const MessageInterface> source,
    shared_ptr<const MessageInterface> destination, InitPayload *initPayload)
    : DeviceMessage(source, destination), initPayload(initPayload) {}

shared_ptr<InitPayload> InitDeviceMessage::returnPayload() {
  return this->initPayload;
}

string InitDeviceMessage::serialize() { return ""; }

} // namespace Messages
