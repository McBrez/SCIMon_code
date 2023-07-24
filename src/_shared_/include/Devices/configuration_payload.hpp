#ifndef CONFIGURATION_PAYLOAD_HPP
#define CONFIGURATION_PAYLOAD_HPP

// Project includes
#include <data_manager.hpp>>
#include <payload.hpp>
#include <user_id.hpp>

using namespace Messages;
using namespace Utilities;

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
   */
  ConfigurationPayload(
      DeviceType deviceType, const KeyMapping &keyMapping = KeyMapping(),
      const SpectrumMapping &spectrumMapping = SpectrumMapping());

  /**
   * @brief Destroy the Device Configuration object
   */
  virtual ~ConfigurationPayload();

  /**
   * @brief Returns the device type, this object is targeting.
   * @return The targeted device type.
   */
  DeviceType getDeviceType();

  KeyMapping getKeyMapping();

  /**
   * @brief Returns the held spectrum mapping.
   * @return
   */
  SpectrumMapping getSpectrumMapping();

  /**
   * @brief Sets the specturm mapping, that shall be used when the data manager
   * of the targeted device is configured.
   * @param spectrumMapping The spectrum mapping that shall be set.
   */
  void setSpectrumMapping(const SpectrumMapping &spectrumMapping);

  /**
   * @brief Sets the key mapping, that shall be used when the data manager of
   * the targeted device is configured.
   * @param keyMapping The key mapping that shall be set.
   */
  void setKeyMapping(const KeyMapping &keyMapping);

protected:
  /// The device type this object is targeting.
  DeviceType deviceType;
  /// Keys for the data manager of the target to be created.
  KeyMapping keyMapping;
  /// In case keyMapping specifies an spectrum, this has to hold the spectrum
  /// configuration.
  SpectrumMapping spectrumMapping;
};

} // namespace Devices

#endif
