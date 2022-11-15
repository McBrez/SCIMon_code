#ifndef DEVICE_ISX3_HPP
#define DEVICE_ISX3_HPP

// 3rd party includes
#include "windows.h"
#include <ftd2xx.h>

// Project includes
#include <device.hpp>

// The product id of the sciospec ISX3.
#define ISX3_PID 0x89D0

namespace Devices {
class DeviceIsx3 : public Device {
public:
  DeviceIsx3();

  virtual bool configure(DeviceConfiguration *deviceConfiguration) override;
  virtual bool open() override;
  virtual bool close() override;

  bool isConfigured();
  virtual bool write(shared_ptr<InitDeviceMessage> initMsg) override;
  virtual bool write(shared_ptr<WriteDeviceMessage> writeMsg) override;
  virtual shared_ptr<ReadDeviceMessage> read() override;

private:
  /// Reference to a handle to the device.
  shared_ptr<FT_HANDLE> handle;

  char readAck();
};
} // namespace Devices

#endif