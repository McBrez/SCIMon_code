#ifndef DEVICE_HPP
#define DEVICE_HPP

// Standard includes
#include <list>
#include <memory>

// Project includes
#include <config_device_message.hpp>
#include <device_configuration.hpp>
#include <init_device_message.hpp>
#include <read_device_message.hpp>
#include <write_device_message.hpp>

using namespace std;

namespace Devices {

/**
 * @brief Depicts a physical device, like a measurement aparatus or a
 * pump controller. A device can be configured, can be read to and written
 * from.
 */
class Device {
protected:
  bool configurationFinished;
  bool initFinished;
  shared_ptr<DeviceConfiguration> deviceConfiguration;

public:
  Device();

  virtual bool configure(DeviceConfiguration *deviceConfiguration) = 0;
  virtual bool open() = 0;
  virtual bool close() = 0;

  bool isConfigured();
  bool isInitialized();

  virtual bool write(shared_ptr<InitDeviceMessage>) = 0;
  virtual bool write(shared_ptr<ConfigDeviceMessage>) = 0;
  virtual bool write(shared_ptr<WriteDeviceMessage>) = 0;

  virtual shared_ptr<ReadDeviceMessage> read() = 0;
  list<shared_ptr<DeviceMessage>> readN(unsigned int n);
};
} // namespace Devices

#endif
