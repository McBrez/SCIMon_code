#ifndef DEVICE_OB1_WIN_HPP
#define DEVICE_OB1_WIN_HPP

// Standard includes
#include <map>
#include <thread>

// Project includes
#include <device_ob1.hpp>

using namespace Utilities;

namespace Devices {
/**
 * @brief Encapsulates the windows implementation of the Elveflow OB1 device.
 */
class DeviceOb1Win : public DeviceOb1 {
public:
  /**
   * @brief Constructs the object.
   */
  DeviceOb1Win();

  /**
   * @brief Destroys the object.
   */
  virtual ~DeviceOb1Win() override;

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
   * @brief Starts the OB1 device and restores the previously set channel
   * pressures. If no channel pressures have been set before, the are set to 0
   * mBar.
   * @return TRUE if device was started. False otherwise.
   */
  virtual bool start() override;

  /**
   * @brief Stops the OB1 device and caches the currently set channel
   * pressures.
   * @return TRUE if device was started. False otherwise.
   */
  virtual bool stop() override;

  /**
   * @brief Handles a device specific message. Called by
   * write std::shared_ptr<WriteDeviceMessage>), if the mssage could not be
   * resolved.
   * @param writeMsg The device specific message.
   * @return True if succesful. False otherwise.
   */
  virtual bool
  specificWrite(std::shared_ptr<WriteDeviceMessage> writeMsg) override;

  /**
   * @brief Device-specific read operation, that is executed on each call of
   * read(). Appends to the outgoing message queue.
   * @param timestamp The time at which this method is called.
   * @return A read message.
   */
  virtual std::list<std::shared_ptr<DeviceMessage>>
  specificRead(TimePoint timestamp) override;

private:
  /**
   * @brief Thread worker for the configuration process.
   * @param deviceConfiguration Holds the configuration data.
   */
  void
  configureWorker(std::shared_ptr<ConfigurationPayload> deviceConfiguration);

  /// Id of the OB1 device. Used by the OB1 driver.
  int ob1Id;

  /// Array of double values holding the calibration.
  double *calibration;

  /// Reference to a thread, that calls the configuration() method.
  std::unique_ptr<std::thread> configurationThread;

  /// Cache for the channel pressures, used to restore the previous pressures,
  /// when start() is called.
  std::map<int, double> cachedPressures;
};
} // namespace Devices

#endif