#ifndef DEVICE_OB1_HPP
#define DEiCE_OB1_HPP

#include <device.hpp>

namespace Devices {
class DeviceOb1 : public Device {
public:
  DeviceOb1();
  virtual bool configure(DeviceConfiguration *deviceConfiguration) override;
  virtual bool open() override;
  virtual bool close() override;

  bool isConfigured();
  bool isInitialized();

  virtual bool write(shared_ptr<InitDeviceMessage>) override;
  virtual bool write(shared_ptr<ConfigDeviceMessage>) override;
  virtual bool write(shared_ptr<WriteDeviceMessage>) override;

  virtual shared_ptr<ReadDeviceMessage> read() override;
  list<shared_ptr<DeviceMessage>> readN(unsigned int n);
};
} // namespace Devices

#endif