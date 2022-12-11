#ifndef DEVICE_HPP
#define DEVICE_HPP

// Standard includes
#include <list>
#include <memory>
#include <queue>

// Project includes
#include <device_configuration.hpp>
#include <message_interface.hpp>

using namespace std;
using namespace Messages;
using namespace Utilities;

namespace Devices {

enum DeviceStatus {
  UNKNOWN_DEVICE_STATUS,
  INIT,
  CONFIGURE,
  OPERATING,
  IDLE,
  BUSY
};

/**
 * @brief Depicts a physical device, like a measurement aparatus or a
 * pump controller. A device can be configured, can be read to and written
 * from.
 */
class Device : public MessageInterface {
protected:
  /// Flag, that indicates whether the device has been successfully configured.
  bool configurationFinished;
  /// Flag, that indicates whether the device has been succesfully initialized.
  bool initFinished;
  /// Reference to the currently active device configuration.
  shared_ptr<DeviceConfiguration> deviceConfiguration;
  /// The state of the device.
  DeviceStatus deviceState;

public:
  /**
   * @brief Construct a new Device object
   */
  Device();

  /**
   * Destroys the device object.
   */
  virtual ~Device() = 0;

  /**
   * Configures the device according to the given configuration.
   *
   * @param deviceConfiguration The configuration that shall be applied to the
   * device.
   * @return TRUE if configuration was successful. False otherwise.
   */
  virtual bool
  configure(shared_ptr<DeviceConfiguration> deviceConfiguration) = 0;

  /**
   * @brief Starts the operation of the device, provided that there is an valid
   * configuration.
   * @return TRUE if device has been started. FALSE if an error occured.
   */
  virtual bool start() = 0;

  /**
   * @brief Stops the operation of the device.
   * @return TRUE if device has been started. FALSE if an error occured.
   */
  virtual bool stop() = 0;

  /**
   * @brief Retruns whether the device has been succesfully configured.
   * @return True if the device has been successfully configured false
   * otherwise.
   */
  bool isConfigured();

  /**
   * @brief Retruns whether the device has been succesfully initialized.
   * @return True if the device has been successfully initialized false
   * otherwise.
   */
  bool isInitialized();

  /**
   * @brief Return the name of the device type.
   * @return The device type name.
   */
  virtual string getDeviceTypeName() = 0;

  /**
   * @brief Returns the status of the device.
   * @return The status of the device.
   */
  DeviceStatus getDeviceStatus();

  // ----------------------------------------------------- Message Interface --
  // Pull the write() methods into scope, that are not defined here. Otherwise,
  // they could be hidden, if only a sub set of the write() methods are defined
  // here.
  using MessageInterface::write;

  /**
   * @brief Writes a message to the device.
   * @param writeMsg The message that shall be written to the device.
   * @return True if successful. False otherwise.
   */
  virtual bool write(shared_ptr<WriteDeviceMessage> writeMsg) override;

  /**
   * @brief Reads all messages from the message queue.
   * @param timestamp The time at which this method is called.
   * @return List of references to the messages from the message queue. May
   * return an empty list, if there was nothing to read.
   */
  virtual list<shared_ptr<DeviceMessage>> read(TimePoint timestamp) override;
};
} // namespace Devices

#endif
