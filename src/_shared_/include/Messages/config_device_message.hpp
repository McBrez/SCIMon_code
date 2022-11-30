#ifndef CONFIG_DEVICE_MESSAGE_HPP
#define CONFIG_DEVICE_MESSAGE_HPP

// Project includes
#include <device_configuration.hpp>
#include <write_device_message.hpp>

using namespace Devices;

namespace Messages {
/**
 * A message that indicates to a device that it shall configure itself according
 * to the given device configuration.
 */
class ConfigDeviceMessage : public WriteDeviceMessage {
public:
  /**
   * @brief Construct a new ConfigDeviceMessage
   * @param deviceConfiguration pointer to an existing device configuration.
   * Object takes ownership of the pointer.
   */
  ConfigDeviceMessage(DeviceConfiguration *deviceConfiguration);
  shared_ptr<DeviceConfiguration> getConfiguration();

protected:
  shared_ptr<DeviceConfiguration> deviceConfiguration;
};
} // namespace Messages

#endif