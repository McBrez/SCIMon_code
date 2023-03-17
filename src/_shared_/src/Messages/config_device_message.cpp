#include <config_device_message.hpp>

namespace Messages {

ConfigDeviceMessage::ConfigDeviceMessage(
    shared_ptr<MessageInterface> source,
    ConfigurationPayload *deviceConfiguration)
    : DeviceMessage(source), deviceConfiguration(deviceConfiguration) {}

ConfigDeviceMessage::~ConfigDeviceMessage() {}

shared_ptr<ConfigurationPayload> ConfigDeviceMessage::getConfiguration() {
  return this->deviceConfiguration;
}

string ConfigDeviceMessage::serialize() { return ""; }

} // namespace Messages