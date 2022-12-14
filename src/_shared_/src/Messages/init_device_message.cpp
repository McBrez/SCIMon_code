// Project include
#include <init_device_message.hpp>

namespace Messages {
InitDeviceMessage::InitDeviceMessage(shared_ptr<MessageInterface> source,
                                     InitPayload *initPayload,
                                     UserId targetUserId)
    : DeviceMessage(source), initPayload(initPayload),
      targetUserId(targetUserId) {}

shared_ptr<InitPayload> InitDeviceMessage::returnPayload() {
  return this->initPayload;
}

UserId InitDeviceMessage::getTargetUserId() { return this->targetUserId; }

string InitDeviceMessage::serialize() { return ""; }

} // namespace Messages
