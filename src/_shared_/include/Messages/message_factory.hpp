#ifndef MESSAGE_FACTORY_HPP
#define MESSAGE_FACTORY_HPP

// Project includes
#include <write_device_message.hpp>

namespace Messages {
/**
 * @brief Factory class, that enables convenient creation of messages.
 */
class MessageFactory {
  /**
   * @brief Creates a messages that indicates to an device that it shall start
   * its operation.
   *
   * @return Reference to a message that indicates to an device that it shall
   * start its operation.
   */
  static shared_ptr<WriteDeviceMessage> createRunMessage();
};
} // namespace Messages

#endif