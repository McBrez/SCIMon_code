// Project include
#include <init_device_message.hpp>

namespace Messages {
InitDeviceMessage::InitDeviceMessage(UserId source, UserId destination,
                                     InitPayload *initPayload)
    : DeviceMessage(source, destination), initPayload(initPayload) {}

InitDeviceMessage::InitDeviceMessage(UserId source, UserId destination,
                                     shared_ptr<InitPayload> initPayload)
    : DeviceMessage(source, destination), initPayload(initPayload) {}

InitDeviceMessage::~InitDeviceMessage() {}

shared_ptr<InitPayload> InitDeviceMessage::returnPayload() {
  return this->initPayload;
}

string InitDeviceMessage::serialize() { return ""; }

} // namespace Messages
