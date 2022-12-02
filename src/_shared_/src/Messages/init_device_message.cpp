// Project include
#include <init_device_message.hpp>

namespace Messages {
InitDeviceMessage::InitDeviceMessage() {}

void InitDeviceMessage::setPayload(InitPayload *initPayload) {
  this->payload.reset(initPayload);
}

shared_ptr<InitPayload> InitDeviceMessage::returnPayload() {
  return this->payload;
}

string InitDeviceMessage::serialize() { return ""; }

} // namespace Messages
