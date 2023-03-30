#ifndef DEVICE_OB1_HPP
#define DEVICE_OB1_HPP

#include <device.hpp>

namespace Devices {
/**
 * @brief Provides the implementation of the interface to the OB1 microfluidics
 * pump controller.
 */
class DeviceOb1 : public Device {
public:
  /**
   * @brief Construct the object.
   */
  DeviceOb1();

  /**
   * @brief Destroy the object.
   */
  virtual ~DeviceOb1() override;

  /**
   * @brief Return the name of the device type.
   * @return The device type name.
   */
  virtual string getDeviceTypeName() override;

  /**
   * @brief Handles the response to a write message that has been sent by this
   * object.
   * @param response The response to a write message that has been sent earlier.
   * @return TRUE if the response has been handled successfully. False
   * otherwise.
   */
  bool handleResponse(shared_ptr<ReadDeviceMessage> response) override;
};
} // namespace Devices

#endif