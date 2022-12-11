#ifndef DEVICE_OB1_WIN_HPP
#define DEVICE_OB1_WIN_HPP

// Standard includes
#include <map>
#include <thread>

// Project includes
#include <device_ob1.hpp>

using namespace std;
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
   * @brief Initializes the device with the given init structure.
   * @return TRUE if initialization was succesfull. False otherwise.
   */
  bool init();

  virtual bool
  configure(shared_ptr<DeviceConfiguration> deviceConfiguration) override;

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

  // Pull all write methods into scope.
  using Device::write;
  /**
   * @brief Writes an initialization message to the device.
   * @param initMsg The initialization message. that shall be written to the
   * device.
   * @return True if the initialization message has been received successfully.
   * False otherwise.
   */
  virtual bool write(shared_ptr<InitDeviceMessage> initMsg) override;

  /**
   * @brief Writes a configuration message to the device.
   * @param configMsg The configuration message. that shall be written to the
   * device.
   * @return True if the configuration message has been received successfully.
   * False otherwise.
   */
  virtual bool write(shared_ptr<ConfigDeviceMessage> configMsg) override;

  /**
   * @brief Handles a device specific message. Called by
   * write(shared_ptr<WriteDeviceMessage>), if the mssage could not be resolved.
   * @param writeMsg The device specific message.
   * @return True if succesful. False otherwise.
   */
  virtual bool specificWrite(shared_ptr<WriteDeviceMessage> writeMsg) override;

  /**
   * @brief Device-specific read operation, that is executed on each call of
   * read(). Appends to the outgoing message queue.
   * @param timestamp The time at which this method is called.
   * @return A read message.
   */
  virtual list<shared_ptr<DeviceMessage>>
  specificRead(TimePoint timestamp) override;

private:
  /// Id of the OB1 device. Used by the OB1 driver.
  int ob1Id;

  /// Array of double values holding the calibration.
  double *calibration;

  /// Reference to a thread, that calls the configuration() method.
  unique_ptr<thread> configurationThread;

  /// Cache for the channel pressures, used to restore the previous pressures,
  /// when start() is called.
  map<int, double> cachedPressures;
};
} // namespace Devices

#endif