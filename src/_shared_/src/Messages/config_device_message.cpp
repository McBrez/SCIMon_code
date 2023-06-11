#include <config_device_message.hpp>

namespace Messages {

ConfigDeviceMessage::ConfigDeviceMessage(
    UserId source, UserId destination,
    ConfigurationPayload *deviceConfiguration, UserId responseId)
    : DeviceMessage(source, destination),
      deviceConfiguration(deviceConfiguration), responseId(responseId) {}

ConfigDeviceMessage::ConfigDeviceMessage(
    UserId source, UserId destination,
    std::shared_ptr<ConfigurationPayload> deviceConfiguration,
    UserId responseId)
    : DeviceMessage(source, destination),
      deviceConfiguration(deviceConfiguration), responseId(responseId) {}

ConfigDeviceMessage::~ConfigDeviceMessage() {}

std::shared_ptr<ConfigurationPayload> ConfigDeviceMessage::getConfiguration() {
  return this->deviceConfiguration;
}

std::string ConfigDeviceMessage::serialize() { return ""; }

UserId ConfigDeviceMessage::getResponseId() const { return this->responseId; }

} // namespace Messages