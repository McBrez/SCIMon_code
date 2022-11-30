#include <device_configuration.hpp>

namespace Devices {
DeviceConfiguration::DeviceConfiguration(DeviceType deviceType)
    : deviceType(deviceType) {}

DeviceConfiguration::~DeviceConfiguration() {}

DeviceType DeviceConfiguration::getDeviceType() { return this->deviceType; }
} // namespace Devices