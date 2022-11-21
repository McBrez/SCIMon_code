// Project includes
#include <read_device_message.hpp>

namespace Devices {

ReadDeviceMessage::ReadDeviceMessage(string data) : data(data) {}

string ReadDeviceMessage::getData() { return this->data; }

string ReadDeviceMessage::serialize() { return this->data; }

} // namespace Devices