#ifndef DEVICE_HPP
#define DEVICE_HPP

// Standard includes
#include <list>
#include <memory>
#include <queue>

// Project includes
#include <configuration_payload.hpp>

#include <message_interface.hpp>
#include <status_payload.hpp>

using namespace Messages;
using namespace Utilities;

namespace Devices {

/**
 * @brief Depicts a physical device, like a measurement aparatus or a
 * pump controller. A device can be configured, can be read to and written
 * from.
 */
class Device : public MessageInterface {

public:
  /**
   * @brief Construct a new Device object
   * @param deviceType The device type of the constructed object.
   * @param dataManagerType The type of datamanager that shall be used.
   */
  Device(DeviceType deviceType, DataManagerType dataManagerType =
                                    DataManagerType::DATAMANAGER_TYPE_HDF);

  /**
   * Destroys the device object.
   */
  virtual ~Device() = 0;

  /**
   * @brief Returns the serial number of the device. This is a string that
   * identifies a physical device. For instance, this can be a device serial
   * number, a static network adress, etc.
   * @return The serial number of the device.
   */
  virtual std::string getDeviceSerialNumber() = 0;

  /**
   * @brief Constructs the current status of the object.
   * @return Pointer to the current status of the object.
   */
  virtual std::shared_ptr<StatusPayload> constructStatus() override;

  /**
   * @brief Returns the string representation of the given device status.
   * @param deviceStatus The numeric device status value that shall be converted
   * to string.
   * @return The string representation of the device status value.
   */
  static std::string deviceStatusToString(DeviceStatus deviceStatus);

protected:
  /// Caches the message that triggered the most recent start command.
  std::shared_ptr<WriteDeviceMessage> startMessageCache;
  /// Flag, that indicates whether the device has been successfully configured.
  bool configurationFinished;
  /// Flag, that indicates whether the device has been succesfully initialized.
  bool initFinished;
};
} // namespace Devices

#endif
