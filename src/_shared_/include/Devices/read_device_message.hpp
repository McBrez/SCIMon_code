#ifndef READ_DEVICE_MESSAGE_HPP
#define READ_DEVICE_MESSAGE_HPP

// Project includes
#include <device_message.hpp>

namespace Devices {
/**
 * @brief Encapsulates a message that shall trigger a reset and initialization
 * of a device.
 */
class ReadDeviceMessage : public DeviceMessage {
public:
  ReadDeviceMessage(string data);
  string getData();
  virtual string serialize() override;

private:
  string data;
};
} // namespace Devices

#endif