#include <config_device_message.hpp>

namespace Messages {

ConfigDeviceMessage::ConfigDeviceMessage(
    shared_ptr<MessageInterface> source,
    shared_ptr<MessageInterface> destination,
    ConfigurationPayload *deviceConfiguration)
    : DeviceMessage(source, destination),
      deviceConfiguration(deviceConfiguration) {}

ConfigDeviceMessage::ConfigDeviceMessage(
    shared_ptr<MessageInterface> source,
    shared_ptr<MessageInterface> destination,
    shared_ptr<ConfigurationPayload> deviceConfiguration)
    : DeviceMessage(source, destination),
      deviceConfiguration(deviceConfiguration) {}

ConfigDeviceMessage::~ConfigDeviceMessage() {}

shared_ptr<ConfigurationPayload> ConfigDeviceMessage::getConfiguration() {
  return this->deviceConfiguration;
}

string ConfigDeviceMessage::serialize() { return ""; }

} // namespace Messages