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
   * @param deviceConfiguration pointer to a new device configuration. Object
   * takes ownership of the pointer.
   * @param responseId The id to which the primary response shall be written to.
   * (I.e. after start() has been called on the device.)
   */
  ConfigDeviceMessage(UserId source, UserId destination,
                      ConfigurationPayload *deviceConfiguration,
                      UserId responseId = UserId());

  /**
   * @brief Constructor for payloads that already exist.
   * @param source reference to the source of this message.
   * @param destination reference to the destination of this message.
   * @param deviceConfiguration pointer to an existing device configuration.
   * @param responseId The id to which the primary response shall be written to.
   * (I.e. after start() has been called on the device.)
   */
  ConfigDeviceMessage(UserId source, UserId destination,
                      shared_ptr<ConfigurationPayload> deviceConfiguration,
                      UserId responseId = UserId());

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
   * @brief Returns the response id.
   * @return The response id.
   */
  UserId getResponseId() const;

  /**
   * @brief Serializes the message into a string.
   *
   * @return The serialized message.
   */
  virtual string serialize() override;

protected:
  shared_ptr<ConfigurationPayload> deviceConfiguration;
  /// The id to which the primary response shall be written to. (I.e.after
  /// start() has been called on the device.)
  UserId responseId;
};
} // namespace Messages

#endif