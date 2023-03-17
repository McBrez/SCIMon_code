#include <configuration_payload.hpp>

namespace Devices {
ConfigurationPayload::ConfigurationPayload(DeviceType deviceType)
    : deviceType(deviceType) {}

ConfigurationPayload::~ConfigurationPayload() {}

DeviceType ConfigurationPayload::getDeviceType() { return this->deviceType; }
} // namespace Devices