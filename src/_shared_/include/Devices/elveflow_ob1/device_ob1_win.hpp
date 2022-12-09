#ifndef DEVICE_OB1_WIN_HPP
#define DEVICE_OB1_WIN_HPP

// Standard includes
#include <thread>

// Project includes
#include <device_ob1.hpp>

using namespace std;

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
   *
   * @return TRUE if device was started. False otherwise.
   */
  virtual bool start() override;
  virtual bool stop() override;

  virtual bool write(shared_ptr<InitDeviceMessage> initMsg) override;
  virtual bool write(shared_ptr<ConfigDeviceMessage> configMsg) override;
  virtual bool specificWrite(shared_ptr<WriteDeviceMessage> writeMsg) override;
  virtual shared_ptr<ReadDeviceMessage> specificRead() override;

private:
  /// Id of the OB1 device. Used by the OB1 driver.
  int ob1Id;

  /// Array of double values holding the calibration.
  double *calibration;

  /// Reference to a thread, that calls the configuration() method.
  unique_ptr<thread> configurationThread;
};
} // namespace Devices

#endif