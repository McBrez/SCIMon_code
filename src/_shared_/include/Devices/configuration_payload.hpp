#ifndef CONFIGURATION_PAYLOAD_HPP
#define CONFIGURATION_PAYLOAD_HPP

// Project includes
#include <common.hpp>
#include <data_manager.hpp>
#include <payload.hpp>
#include <user_id.hpp>

using namespace Messages;
using namespace Utilities;

namespace Devices {

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

  /**
   * @brief Returns the string representation of the given device type.
   * @param type The device type whose text representation shall be retrieved.
   * @return The string representation of the given device type.
   */
  static std::string deviceTypeToString(DeviceType type);

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
