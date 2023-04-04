// Standard includes
#include <climits>
#include <iostream>

// Project includes
#include <device_message.hpp>

using namespace std;

namespace Messages {

DeviceMessage::DeviceMessage(shared_ptr<MessageInterface> source,
                             shared_ptr<MessageInterface> destination)
    : messageId(this->generateId()), source(source), destination(destination) {}

DeviceMessage::~DeviceMessage() {}

shared_ptr<DeviceMessage> DeviceMessage::deserialize(string data) {
  return shared_ptr<DeviceMessage>();
}

int DeviceMessage::getMessageId() { return this->messageId; }

int DeviceMessage::generateId() {
  // TODO: IDs should not only be unique within an application execution but
  // also over different proceses. Maybe use an UUID?
  static int currentId = 0;
  if (currentId == INT_MAX) {
    return -1;
  } else {
    return currentId++;
  }
}

shared_ptr<MessageInterface> DeviceMessage::getSource() { return this->source; }

shared_ptr<MessageInterface> DeviceMessage::getDestination() {
  return this->destination;
}

void DeviceMessage::setSource(shared_ptr<MessageInterface> source) {
  this->source = source;
}
void DeviceMessage::setDestination(shared_ptr<MessageInterface> destination) {
  this->destination = destination;
}

} // namespace Messages