#ifndef DEVICE_HPP
#define DEVICE_HPP

// Standard includes
#include <list>
#include <memory>
#include <queue>

// Project includes
#include <config_device_message.hpp>
#include <device_configuration.hpp>
#include <init_device_message.hpp>
#include <read_device_message.hpp>
#include <write_device_message.hpp>

using namespace std;
using namespace Messages;

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
class Device {
protected:
  /// Flag, that indicates whether the device has been successfully configured.
  bool configurationFinished;
  /// Flag, that indicates whether the device has been succesfully initialized.
  bool initFinished;
  /// Reference to the currently active device configuration.
  shared_ptr<DeviceConfiguration> deviceConfiguration;
  /// The unique id of the device.
  DeviceId deviceId;
  /// The state of the device.
  DeviceStatus deviceState;

  /// Queue for outgoing messages.
  queue<shared_ptr<ReadDeviceMessage>> messageOut;

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

  bool isConfigured();

  bool isInitialized();

  /**
   * @brief Return the name of the device type.
   * @return The device type name.
   */
  virtual string getDeviceTypeName() = 0;

  /**
   * @brief Returns the unique id of the device.
   * @return The unique id of the device.
   */
  DeviceId getDeviceId();

  /**
   * @brief Returns the status of the device.
   * @return The status of the device.
   */
  DeviceStatus getDeviceStatus();

  /**
   * @brief Writes an initialization message to the device.
   * @param initMsg The initialization message. that shall be written to the
   * device.
   * @return True if the initialization message has been received successfully.
   * False otherwise.
   */
  virtual bool write(shared_ptr<InitDeviceMessage> initMsg) = 0;

  /**
   * @brief Writes a configuration message to the device.
   * @param configMsg The configuration message. that shall be written to the
   * device.
   * @return True if the configuration message has been received successfully.
   * False otherwise.
   */
  virtual bool write(shared_ptr<ConfigDeviceMessage> configMsg) = 0;

  /**
   * @brief Writes a message to the device.
   * @param writeMsg The message that shall be written to the device.
   * @return True if successful. False otherwise.
   */
  virtual bool write(shared_ptr<WriteDeviceMessage> writeMsg);
  /**
   * @brief Handles a device specific message. Called by
   * write(shared_ptr<WriteDeviceMessage>), if the mssage could not be resolved.
   * @param writeMsg The device specific message.
   * @return True if succesful. False otherwise.
   */
  virtual bool specificWrite(shared_ptr<WriteDeviceMessage> writeMsg) = 0;
  /**
   * @brief Reads one message from the message queue.
   * @return Reference to the next message from the message queue. May return an
   * empty pointer, if there was nothing to read.
   */
  virtual shared_ptr<ReadDeviceMessage> read();

  /**
   * @brief Device-specific read operation, that is executed on each call of
   * read(). Appends to the outgoing message queue.
   * @return A read message.
   */
  virtual shared_ptr<ReadDeviceMessage> specificRead() = 0;

  list<shared_ptr<DeviceMessage>> readN(unsigned int n);
};
} // namespace Devices

#endif
