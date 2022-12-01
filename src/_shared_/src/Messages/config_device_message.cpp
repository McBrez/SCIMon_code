#include <config_device_message.hpp>

namespace Messages {

ConfigDeviceMessage::ConfigDeviceMessage(
    DeviceConfiguration *deviceConfiguration)
    : deviceConfiguration(deviceConfiguration) {}

shared_ptr<DeviceConfiguration> ConfigDeviceMessage::getConfiguration() {
  return this->deviceConfiguration;
}

string ConfigDeviceMessage::serialize() { return ""; }

} // namespace Messages