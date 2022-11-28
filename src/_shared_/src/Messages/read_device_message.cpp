// Project includes
#include <read_device_message.hpp>

namespace Messages {

ReadDeviceMessage::ReadDeviceMessage(string data) : data(data) {}

string ReadDeviceMessage::getData() { return this->data; }

string ReadDeviceMessage::serialize() { return this->data; }

} // namespace Messages