#ifndef DEVICE_OB1_HPP
#define DEiCE_OB1_HPP

#include <device.hpp>

namespace Devices {
/**
 * @brief Provides the implementation of the interface to the OB1 microfluidics
 * pump controller.
 */
class DeviceOb1 : public Device {
public:
  DeviceOb1();
  virtual ~DeviceOb1() override;
  virtual bool
  configure(shared_ptr<DeviceConfiguration> deviceConfiguration) override;
  virtual bool start() override;
  virtual bool stop() override;

  bool isConfigured();
  bool isInitialized();

  virtual bool write(shared_ptr<InitDeviceMessage>) override;
  virtual bool write(shared_ptr<ConfigDeviceMessage>) override;
  virtual bool write(shared_ptr<WriteDeviceMessage>) override;

  /**
   * @brief Return the name of the device type.
   *
   * @return The device type name.
   */
  virtual string getDeviceTypeName() override;

  virtual shared_ptr<ReadDeviceMessage> read() override;
};
} // namespace Devices

#endif