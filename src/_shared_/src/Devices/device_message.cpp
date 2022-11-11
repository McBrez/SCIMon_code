#include <iostream>

#include <device_message.hpp>

namespace Devices {
void DeviceMessage::getPayload() { std::cout << this->payload << std::endl; }
} // namespace Devices