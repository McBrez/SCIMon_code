#include <config_device_message.hpp>

namespace Messages {

ConfigDeviceMessage::ConfigDeviceMessage(
    UserId source, UserId destination,
    ConfigurationPayload *deviceConfiguration)
    : DeviceMessage(source, destination),
      deviceConfiguration(deviceConfiguration) {}

ConfigDeviceMessage::ConfigDeviceMessage(
    UserId source, UserId destination,
    shared_ptr<ConfigurationPayload> deviceConfiguration)
    : DeviceMessage(source, destination),
      deviceConfiguration(deviceConfiguration) {}

ConfigDeviceMessage::~ConfigDeviceMessage() {}

shared_ptr<ConfigurationPayload> ConfigDeviceMessage::getConfiguration() {
  return this->deviceConfiguration;
}

string ConfigDeviceMessage::serialize() { return ""; }

} // namespace Messages