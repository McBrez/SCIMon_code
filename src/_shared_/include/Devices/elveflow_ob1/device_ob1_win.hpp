#ifndef DEVICE_OB1_WIN_HPP
#define DEVICE_OB1_WIN_HPP

#include <device_ob1.hpp>

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
   *
   * @return TRUE if initialization was succesfull. False otherwise.
   */
  bool init();

  virtual bool
  configure(shared_ptr<DeviceConfiguration> deviceConfiguration) override;
  virtual bool start() override;
  virtual bool stop() override;

  virtual bool write(shared_ptr<InitDeviceMessage>) override;
  virtual bool write(shared_ptr<ConfigDeviceMessage>) override;
  virtual bool write(shared_ptr<WriteDeviceMessage>) override;

  virtual shared_ptr<ReadDeviceMessage> read() override;

private:
  int MyOB1_ID;
  double *calibration;
};
} // namespace Devices

#endif