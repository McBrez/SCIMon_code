#ifndef DEVICE_ID_HPP
#define DEVICE_ID_HPP

namespace Devices {
/**
 * @brief Encapsulate a device id.
 */
class DeviceId {
public:
  /**
   * @brief Creates an device id with an automatically generated id.
   */
  DeviceId();

  /**
   * @brief Creates an device id with the given id.
   * @param deviceId: The id of the object.
   */
  DeviceId(int deviceId);

  /**
   * @brief Returns the id.
   * @return The id.
   */
  int id() const;

  /**
   * @brief Checks if two device ids are equal.
   *
   * @param other The other device id.
   * @return True if device ids are equal. False otherwise.
   */
  bool operator==(const DeviceId &other);

  /**
   * @brief Checks if two device ids are unequal.
   *
   * @param other The other device id.
   * @return True if device ids are unequal. False otherwise.
   */
  bool operator!=(const DeviceId &other);

private:
  int deviceId;
  int generateDeviceId();
};
} // namespace Devices

#endif