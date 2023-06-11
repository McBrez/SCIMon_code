#ifndef DUMMY_DEVICE_HPP
#define DUMMY_DEVICE_HPP

// Project includes
#include <device.hpp>

namespace Devices {

class DummyDevice : public Device {
public:
  DummyDevice();

  virtual ~DummyDevice();
  /**
   * @brief Starts the operation of the device, provided that there is an valid
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
   * Initializes the device according to the given configuration.
   * @param deviceConfiguration The configuration that shall be applied to the
   * device.
   * @return TRUE if configuration was successful. False otherwise.
   */
  virtual bool initialize(std::shared_ptr<InitPayload> initPayload) override;

  /**
   * Configures the device according to the given configuration.
   * @param deviceConfiguration The configuration that shall be applied to the
   * device.
   * @return TRUE if configuration was successful. False otherwise.
   */
  virtual bool
  configure(std::shared_ptr<ConfigurationPayload> deviceConfiguration) override;

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

  virtual bool
  handleResponse(std::shared_ptr<ReadDeviceMessage> response) override;

  /**
   * @brief Return the name of the device type.
   * @return The device type name.
   */
  virtual std::string getDeviceTypeName() override;
};
} // namespace Devices

#endif