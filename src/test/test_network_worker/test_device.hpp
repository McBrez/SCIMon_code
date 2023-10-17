#ifndef TEST_DEVICE_HPP
#define TEST_DEVICE_HPP

// Project includes
#include <device.hpp>

using namespace Utilities;

namespace Devices {

/**
 * @brief Depcits a dummy device, that can be used for unit tests.
 */
class TestDevice : public Device {
public:
  /**
   * @brief Construct a new Device Isx 3 object
   */
  TestDevice();

  /**
   * @brief Destroy the Device Isx 3 object
   */
  virtual ~TestDevice() override;

  /**
   * @brief Return the name of the device type.
   * @return The device type name.
   */
  virtual std::string getDeviceTypeName() override;

  /**
   * @brief Handles a device specific write operation.
   * @param writeMsg The specific message.
   * @return True if the write operation was successfull. False otherwise.
   */
  virtual bool
  specificWrite(std::shared_ptr<WriteDeviceMessage> writeMsg) override;

  /**
   * @brief Handles a device specific read operation.
   * @param timestamp The timestamp with which this operation is called.
   * @return The response to the specific read operation.
   */
  virtual std::list<std::shared_ptr<DeviceMessage>>
  specificRead(TimePoint timestamp) override;

  /**
   * @brief Handles the response to a write message that has been sent by this
   * object.
   * @param response The response to a write message that has been sent earlier.
   * @return TRUE if the response has been handled successfully. False
   * otherwise.
   */
  virtual bool
  handleResponse(std::shared_ptr<ReadDeviceMessage> response) override;

  /**
   * Configures the device according to the given configuration.
   * @param deviceConfiguration The configuration that shall be applied to the
   * device.
   * @return TRUE if configuration was successful. False otherwise.
   */
  virtual bool
  configure(std::shared_ptr<ConfigurationPayload> deviceConfiguration) override;

  /**
   * Initializes the device according to the given configuration.
   * @param deviceConfiguration The configuration that shall be applied to the
   * device.
   * @return TRUE if configuration was successful. False otherwise.
   */
  virtual bool initialize(std::shared_ptr<InitPayload> initPayload) override;

  /**
   * @brief Starts the operation of the device, provided that there is a valid
   * configuration.
   * @return TRUE if device has been started. FALSE if an error occured.
   */
  virtual bool start() override;

  /**
   * @brief Stops the operation of the device.
   * @return TRUE if device has been started. FALSE if an error occured.
   */
  virtual bool stop() override;

  /**
   * @brief Returns the serial number of the device. This is a string that
   * identifies a physical device. For instance, this can be a device serial
   * number, a static network adress, etc.
   * @return The serial number of the device.
   */
  virtual std::string getDeviceSerialNumber() override;

  std::vector<unsigned char> returnReceivedVector();

private:
  /// String that identifies this type of device.
  static const std::string TEST_DEVICE_TYPE_NAME;
  UserId remoteTestDeviceId;
  std::vector<unsigned char> receivedVector;
};
} // namespace Devices

#endif
