#ifndef ID_PAYLOAD_HPP
#define ID_PAYLOAD_HPP

// Project includes.
#include <device_configuration.hpp>
#include <read_payload.hpp>

namespace Devices {
/**
 * @brief Encapsulates the ID of a device.S
 */
class IdPayload : public ReadPayload {
public:
  IdPayload(string manufacturer, DeviceType deviceType, int deviceIdentifier,
            int serialNumber);

  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual string serialize() override;

private:
  /// The manufacturer of the device.
  string manufacturer;
  /// The type of the device.
  DeviceType deviceType;
  /// The identifier for the device type.
  int deviceIdentifier;
  /// The serial number of the device.
  int serialNumber;
};
} // namespace Devices

#endif