#ifndef ID_PAYLOAD_HPP
#define ID_PAYLOAD_HPP

// Project includes.
#include <configuration_payload.hpp>
#include <read_payload.hpp>

namespace Devices {
/**
 * @brief Encapsulates the ID of a device.
 */
class IdPayload : public ReadPayload {
public:
  IdPayload(std::string manufacturer, DeviceType deviceType,
            int deviceIdentifier, int serialNumber);

  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual std::string serialize() override;

  /**
   * @brief Serializes the payload into bytes.
   * @return Byte vector, that depicts the payload.
   */
  virtual std::vector<unsigned char> bytes() override;

  /**
   * @brief Returns the magic number of the payload. This number is used to
   * identify the payload type when encoding and decoding payload. This number
   * has to be unique among all payload types.
   */
  virtual int getMagicNumber() override;

  int getDeviceIdentifier() const;

  std::string getManufacturer() const;

  DeviceType getDeviceType() const;

  int getSerialNumber() const;

private:
  /// The manufacturer of the device.
  std::string manufacturer;
  /// The type of the device.
  DeviceType deviceType;
  /// The identifier for the device type.
  int deviceIdentifier;
  /// The serial number of the device.
  int serialNumber;
};
} // namespace Devices

#endif
