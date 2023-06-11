#ifndef STATUS_PAYLOAD_HPP
#define STATUS_PAYLOAD_HPP

// Standard includes
#include <list>

// Project include
#include <configuration_payload.hpp>
#include <read_payload.hpp>
#include <user_id.hpp>

using namespace Messages;

namespace Devices {

/// Identifies the possible stati of a device.
enum DeviceStatus {
  /// Device status is not known.
  UNKNOWN_DEVICE_STATUS = 0x00,
  /// Device is initializing.
  INITIALIZING = 0x01,
  /// Device has been successfully initialized.
  INITIALIZED = 0x02,
  /// Device is configuring.
  CONFIGURING = 0x03,
  /// Device is operating and can accept commands.
  OPERATING = 0x04,
  /// Device is initialized, configured, not operating and is waiting for
  /// commands.
  IDLE = 0x05,
  /// Device is operating and can not accept commands.
  BUSY = 0x06,
  /// Device ran into an error.
  ERROR = 0xFF
};

/**
 * @brief A data package, that encapsulates the state of a device.
 */
class StatusPayload : public ReadPayload {
public:
  /**
   * @brief Constructs the object.
   * @param userId The id of the device, this payload originates from.
   * @param deviceStatus The status of the device.
   */
  StatusPayload(UserId deviceId, DeviceStatus deviceStatus,
                std::list<UserId> proxyIds, DeviceType deviceType,
                std::string deviceName);

  /**
   * @brief Returns the status of the device.
   * @return The status of the device.
   */
  DeviceStatus getDeviceStatus();

  /**
   * @brief Returns the id of the device.
   * @return The id of the device.
   */
  UserId getDeviceId();

  DeviceType getDeviceType();

  std::string getDeviceName();

  std::list<UserId> getProxyIds();

  /**
   * @brief Serializes the payload into a human-readable string.
   * @return The string representation of the payload.
   */
  virtual std::string serialize() override;

  bool operator==(const StatusPayload &other) const;

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

private:
  /// The device status.
  DeviceStatus deviceStatus;

  /// The device id.
  UserId deviceId;

  /// The proxy ids of the device.
  std::list<UserId> proxyIds;

  /// The device type.
  DeviceType deviceType;

  /// The device name.
  std::string deviceName;
};

} // namespace Devices

#endif