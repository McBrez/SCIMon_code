// Project include
#include <init_device_message.hpp>

namespace Messages {
InitDeviceMessage::InitDeviceMessage(InitPayload *initPayload,
                                     DeviceId targetDeviceId)
    : initPayload(initPayload), targetDeviceId(targetDeviceId) {}

shared_ptr<InitPayload> InitDeviceMessage::returnPayload() {
  return this->initPayload;
}

DeviceId InitDeviceMessage::getTargetDeviceId() { return this->targetDeviceId; }

string InitDeviceMessage::serialize() { return ""; }

} // namespace Messages
