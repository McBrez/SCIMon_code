#ifndef CONFIG_DEVICE_MESSAGE_HPP
#define CONFIG_DEVICE_MESSAGE_HPP

// Project includes
#include <device_configuration.hpp>
#include <device_message.hpp>

using namespace Devices;

namespace Messages {
/**
 * A message that indicates to a device that it shall configure itself according
 * to the given device configuration.
 */
class ConfigDeviceMessage : public DeviceMessage {
public:
  /**
   * @brief Construct a new ConfigDeviceMessage
   * @param deviceConfiguration pointer to an existing device configuration.
   * Object takes ownership of the pointer.
   */
  ConfigDeviceMessage(DeviceConfiguration *deviceConfiguration);

  /**
   * @brief Returns a reference to the held configuration.
   *
   * @return The held configuration.
   */
  shared_ptr<DeviceConfiguration> getConfiguration();

  /**
   * @brief Serializes the message into a string.
   *
   * @return The serialized message.
   */
  virtual string serialize() override;

protected:
  shared_ptr<DeviceConfiguration> deviceConfiguration;
};
} // namespace Messages

#endif