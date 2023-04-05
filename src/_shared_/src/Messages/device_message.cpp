// Standard includes
#include <climits>
#include <iostream>
#include <sstream>
#include <string>

// Project includes
#include <device_message.hpp>

using namespace std;

namespace Messages {

DeviceMessage::DeviceMessage(UserId source, UserId destination)
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

UserId DeviceMessage::getSource() { return this->source; }

UserId DeviceMessage::getDestination() { return this->destination; }

void DeviceMessage::setSource(UserId source) { this->source = source; }

void DeviceMessage::setDestination(UserId destination) {
  this->destination = destination;
}

vector<unsigned char> DeviceMessage::bytes() {
  vector<unsigned char>;
  stringstream s;

  UserId sourceId = this->getSource().id();
  s.write(reinterpret_cast<const char *>(&sourceId), sizeof(const char));
  UserId destinationId = this->getDestination().id();
  s.write(reinterpret_cast<const char *>(&destinationId), sizeof(const char));
  s.write(reinterpret_cast<const char *>(&this->messageId), sizeof(const char));

  string *str = new string();
  *str = s.str();
  return vector<unsigned char>((const unsigned char *)str->c_str(),
                               (const unsigned char *)str->c_str() +
                                   str->length());
}

} // namespace Messages