#ifndef DEVICE_CONFIGURATION_HPP
#define DEVICE_CONFIGURATION_HPP

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
class DeviceConfiguration {
public:
  /**
   * @brief Construct a new Device Configuration object
   *
   * @param deviceType The device type this configuration is targeting.
   */
  DeviceConfiguration(DeviceType deviceType);

  /**
   * @brief Destroy the Device Configuration object
   */
  virtual ~DeviceConfiguration();

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