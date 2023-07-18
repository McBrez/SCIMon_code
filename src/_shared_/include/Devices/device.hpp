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
   * Initializes the device according to the given configuration.
   * @param deviceConfiguration The configuration that shall be applied to the
   * device.
   * @return TRUE if configuration was successful. False otherwise.
   */
  virtual bool initialize(std::shared_ptr<InitPayload> initPayload) = 0;

  /**
   * Configures the device according to the given configuration.
   * @param deviceConfiguration The configuration that shall be applied to the
   * device.
   * @return TRUE if configuration was successful. False otherwise.
   */
  virtual bool
  configure(std::shared_ptr<ConfigurationPayload> deviceConfiguration) = 0;

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
  virtual std::string getDeviceTypeName() = 0;

  DeviceType getDeviceType();

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
  virtual bool write(std::shared_ptr<WriteDeviceMessage> writeMsg) override;

  /**
   * @brief Writes a message to the device that triggers an initialization of
   * the device.
   * @param initMsg The init message.
   * @return True if the initialization was succesfull. False otherwise.
   */
  virtual bool write(std::shared_ptr<InitDeviceMessage> initMsg) override;

  /**
   * @brief Writes a message to the device that triggers a configuration.
   * @param configMsg The configuration message.
   * @return True if configuration was succesfull. False otherwise.
   */
  virtual bool write(std::shared_ptr<ConfigDeviceMessage> configMsg) override;

  /**
   * @brief Writes a handshake message to the device.
   * @param writeMsg The handshake message that shall be written to the device.
   * @return True if successful. False otherwise.
   */
  virtual bool write(std::shared_ptr<HandshakeMessage> writeMsg) override;

  /**
   * @brief Reads all messages from the message queue.
   * @param timestamp The time at which this method is called.
   * @return std::list of references to the messages from the message queue. May
   * return an empty std::list, if there was nothing to read.
   */
  virtual std::list<std::shared_ptr<DeviceMessage>>
  read(TimePoint timestamp) override;

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
  /// Reference to the currently active device configuration.
  std::shared_ptr<ConfigurationPayload> deviceConfiguration;
  /// The state of the device.
  DeviceStatus deviceState;
  /// The type of the device.
  DeviceType deviceType;
  /// The id of the response message interface. If this device is started,
  /// messages shall be sent to this object.
  UserId responseId;
};
} // namespace Devices

#endif
