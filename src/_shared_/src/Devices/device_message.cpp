// Standard includes
#include <iostream>

// Project includes
#include <device_message.hpp>

namespace Devices {
void DeviceMessage::getPayload() { std::cout << this->payload << std::endl; }

shared_ptr<DeviceMessage> DeviceMessage::deserialize(string data) {
  return shared_ptr<DeviceMessage>();
}

} // namespace Devices