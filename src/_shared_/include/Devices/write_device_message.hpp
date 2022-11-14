#ifndef WRITE_DEVICE_MESSAGE_HPP
#define WRITE_DEVICE_MESSAGE_HPP

// Project includes
#include <device_message.hpp>

namespace Devices {
/**
 * @brief Encapsulates a message that shall trigger a reset and initialization
 * of a device.
 */
class WriteDeviceMessage : public DeviceMessage {};
} // namespace Devices

#endif