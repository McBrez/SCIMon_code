#ifndef CONFIGURATION_PAYLOAD_HPP
#define CONFIGURATION_PAYLOAD_HPP

// Project includes
#include <payload.hpp>
#include <user_id.hpp>

using namespace Messages;

namespace Devices {
/**
 * @brief Identifies the different device types.
 */
enum DeviceType {
  // Invalid device type.
  INVALID = -1,

  /// Unspecified device type.
  UNSPECIFIED = 0,

  // A pump controller
  PUMP_CONTROLLER = 1,

  // An impedance spectrometer.
  IMPEDANCE_SPECTROMETER = 2
};

/**
 * @brief Base class that encapsulates any device configuration.
 */
class ConfigurationPayload : public Payload {
public:
  /**
   * @brief Construct a new Device Configuration object
   *
   * @param deviceType The device type this configuration is targeting.
   * @param targetId The id of the target message interface. This is the message
   * interface the configured device shall sent messages to, when the device is
   * started.
   */
  ConfigurationPayload(DeviceType deviceType);

  /**
   * @brief Destroy the Device Configuration object
   */
  virtual ~ConfigurationPayload();

  /**
   * @brief Returns the device type, this object is targeting.
   * @return The targeted device type.
   */
  DeviceType getDeviceType();

protected:
  /// The device type this object is targeting.
  DeviceType deviceType;
};

} // namespace Devices

#endif