// Standard includes
#include <climits>
#include <iostream>

// Project includes
#include <device_message.hpp>

namespace Messages {

DeviceMessage::DeviceMessage() : messageId(this->generateId()) {}

shared_ptr<DeviceMessage> DeviceMessage::deserialize(string data) {
  return shared_ptr<DeviceMessage>();
}

int DeviceMessage::getMessageId() { return this->messageId; }

int DeviceMessage::generateId() {
  // TODO: IDs should not only be unique within an application execution but
  // also over different procceses. Maybe use an UUID?
  static int currentId = 0;
  if (currentId == INT_MAX) {
    return -1;
  } else {
    return currentId++;
  }
}

} // namespace Messages