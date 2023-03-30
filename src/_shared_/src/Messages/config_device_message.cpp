#include <config_device_message.hpp>

namespace Messages {

ConfigDeviceMessage::ConfigDeviceMessage(
    shared_ptr<const MessageInterface> source,
    shared_ptr<const MessageInterface> destination,
    ConfigurationPayload *deviceConfiguration)
    : DeviceMessage(source, destination),
      deviceConfiguration(deviceConfiguration) {}

ConfigDeviceMessage::~ConfigDeviceMessage() {}

shared_ptr<ConfigurationPayload> ConfigDeviceMessage::getConfiguration() {
  return this->deviceConfiguration;
}

string ConfigDeviceMessage::serialize() { return ""; }

} // namespace Messages