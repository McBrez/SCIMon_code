// Project include
#include <init_device_message.hpp>

namespace Messages {
InitDeviceMessage::InitDeviceMessage(UserId source, UserId destination,
                                     InitPayload *initPayload)
    : DeviceMessage(source, destination), initPayload(initPayload) {}

InitDeviceMessage::InitDeviceMessage(UserId source, UserId destination,
                                     std::shared_ptr<InitPayload> initPayload)
    : DeviceMessage(source, destination), initPayload(initPayload) {}

InitDeviceMessage::~InitDeviceMessage() {}

std::shared_ptr<InitPayload> InitDeviceMessage::returnPayload() {
  return this->initPayload;
}

std::string InitDeviceMessage::serialize() { return "Init Device Message"; }

} // namespace Messages
