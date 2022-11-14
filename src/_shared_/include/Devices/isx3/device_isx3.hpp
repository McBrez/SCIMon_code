#ifndef DEVICE_ISX3_HPP
#define DEVICE_ISX3_HPP

#include <device.hpp>

namespace Devices {
class DeviceIsx3 : public Device{
public:
  DeviceIsx3();

  virtual bool configure(DeviceConfiguration *deviceConfiguration) override;
  virtual bool open() override;
  virtual bool close() override;

  bool isConfigured();
  virtual bool write(shared_ptr<InitDeviceMessage>) override;
  virtual bool write(shared_ptr<WriteDeviceMessage>) override;
  virtual shared_ptr<ReadDeviceMessage> read() override;
};
} // namespace Devices

#endif