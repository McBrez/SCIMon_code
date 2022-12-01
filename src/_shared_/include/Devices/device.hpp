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
using namespace Messages;

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
  virtual ~Device() = 0;

  /**
   * Configures the device according to the given configuration.
   *
   * @param deviceConfiguration The configuration that shall be applied to the
   * device.
   * @return TRUE if configuration was successful. False otherwise.
   */
  virtual bool
  configure(shared_ptr<DeviceConfiguration> deviceConfiguration) = 0;

  /**
   * @brief Starts the operation of the device, provided that there is an valid
   * configuration.
   *
   * @return TRUE if device has been started. FALSE if an error occured.
   */
  virtual bool start() = 0;

  /**
   * @brief Stops the operation of the device.
   *
   * @return TRUE if device has been started. FALSE if an error occured.
   */
  virtual bool stop() = 0;

  bool isConfigured();
  bool isInitialized();

  /**
   * @brief Return the name of the device type.
   *
   * @return The device type name.
   */
  virtual string getDeviceTypeName() = 0;

  virtual bool write(shared_ptr<InitDeviceMessage>) = 0;
  virtual bool write(shared_ptr<ConfigDeviceMessage>) = 0;
  virtual bool write(shared_ptr<WriteDeviceMessage>) = 0;

  virtual shared_ptr<ReadDeviceMessage> read() = 0;
  list<shared_ptr<DeviceMessage>> readN(unsigned int n);
};
} // namespace Devices

#endif
