#ifndef INIT_DEVICE_MESSAGE_HPP
#define INIT_DEVICE_MESSAGE_HPP

// Project includes
#include <device_message.hpp>

namespace Messages {

/**
 * @brief Encapsulates a message that shall trigger a reset and initialization
 * of a device.
 */
class InitDeviceMessage : public DeviceMessage {};
} // namespace Messages
#endif