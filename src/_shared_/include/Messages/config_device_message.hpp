#ifndef CONFIG_DEVICE_MESSAGE_HPP
#define CONFIG_DEVICE_MESSAGE_HPP

// Project includes
#include <configuration_payload.hpp>
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
   * @param source reference to the source of this message.
   * @param destination reference to the destination of this message.
   * @param deviceConfiguration pointer to an existing device configuration.
   * Object takes ownership of the pointer.
   */
  ConfigDeviceMessage(shared_ptr<const MessageInterface> source,
                      shared_ptr<const MessageInterface> destination,
                      ConfigurationPayload *deviceConfiguration);

  /**
   * @brief Destroy the Config Device Message object
   */
  virtual ~ConfigDeviceMessage() override;

  /**
   * @brief Returns a reference to the held configuration.
   *
   * @return The held configuration.
   */
  shared_ptr<ConfigurationPayload> getConfiguration();

  /**
   * @brief Serializes the message into a string.
   *
   * @return The serialized message.
   */
  virtual string serialize() override;

protected:
  shared_ptr<ConfigurationPayload> deviceConfiguration;
};
} // namespace Messages

#endif