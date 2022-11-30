#ifndef DEVICE_CONFIGURATION_HPP
#define DEVICE_CONFIGURATION_HPP

namespace Devices {
/**
 * @brief Identifies the different device types.
 */
enum DeviceType {
  // Invalid device type.
  INVALID = -1,

  // A pump controller
  PUMP_CONTROLLER = 1,

  // An impedance spectrometer.
  IMPEDANCE_SPECTROMETER
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
  virtual ~DeviceConfiguration() = 0;
  DeviceType getDeviceType();

protected:
  DeviceType deviceType;
};

} // namespace Devices

#endif