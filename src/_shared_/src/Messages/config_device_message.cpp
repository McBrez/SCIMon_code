#include <config_device_message.hpp>

namespace Messages {

ConfigDeviceMessage::ConfigDeviceMessage(
    UserId source, UserId destination,
    ConfigurationPayload *deviceConfiguration, UserId responseId)
    : DeviceMessage(source, destination),
      deviceConfiguration(deviceConfiguration), responseId(responseId) {}

ConfigDeviceMessage::ConfigDeviceMessage(
    UserId source, UserId destination,
    shared_ptr<ConfigurationPayload> deviceConfiguration, UserId responseId)
    : DeviceMessage(source, destination),
      deviceConfiguration(deviceConfiguration), responseId(responseId) {}

ConfigDeviceMessage::~ConfigDeviceMessage() {}

shared_ptr<ConfigurationPayload> ConfigDeviceMessage::getConfiguration() {
  return this->deviceConfiguration;
}

string ConfigDeviceMessage::serialize() { return ""; }

UserId ConfigDeviceMessage::getResponseId() const { return this->responseId; }

} // namespace Messages