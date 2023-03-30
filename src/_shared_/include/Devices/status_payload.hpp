#ifndef STATUS_PAYLOAD_HPP
#define STATUS_PAYLOAD_HPP

// Project include
#include <read_payload.hpp>
#include <user_id.hpp>

using namespace Messages;

namespace Devices {

/// Identifies the possible stati of a device.
enum DeviceStatus {
  /// Device status is not known.
  UNKNOWN_DEVICE_STATUS,
  /// Device is initializing.
  INITIALIZING,
  /// Device has been successfully initialized.
  INITIALIZED,
  /// Device is configuring.
  CONFIGURING,
  /// Device is operating and can accept commands.
  OPERATING,
  /// Device is initialized, configured, not operating and is waiting for
  /// commands.
  IDLE,
  /// Device is operating and can not accept commands.
  BUSY,
  /// Device ran into an error.
  ERROR
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
  StatusPayload(UserId deviceId, DeviceStatus deviceStatus);

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

  /**
   * @brief Serializes the payload into a human-readable string.
   * @return The string representation of the payload.
   */
  virtual string serialize() override;

private:
  /// The device status.
  DeviceStatus deviceStatus;

  /// The device id.
  UserId deviceId;
};

} // namespace Devices

#endif