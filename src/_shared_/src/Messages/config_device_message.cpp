#include <config_device_message.hpp>

namespace Messages {

ConfigDeviceMessage::ConfigDeviceMessage(
    UserId source, UserId destination,
    ConfigurationPayload *deviceConfiguration, std::vector<UserId> responseIds)
    : DeviceMessage(source, destination),
      deviceConfiguration(deviceConfiguration), responseIds(responseIds) {}

ConfigDeviceMessage::ConfigDeviceMessage(
    UserId source, UserId destination,
    std::shared_ptr<ConfigurationPayload> deviceConfiguration,
    std::vector<UserId> responseIds)
    : DeviceMessage(source, destination),
      deviceConfiguration(deviceConfiguration), responseIds(responseIds) {}

ConfigDeviceMessage::~ConfigDeviceMessage() {}

std::shared_ptr<ConfigurationPayload> ConfigDeviceMessage::getConfiguration() {
  return this->deviceConfiguration;
}

std::string ConfigDeviceMessage::serialize() { return ""; }

std::vector<UserId> ConfigDeviceMessage::getResponseIds() const {
  return this->responseIds;
}

} // namespace Messages
