// Project includes
#include <dummy_message.hpp>

namespace Messages {
DummyMessage::DummyMessage()
    : WriteDeviceMessage(UserId(), UserId(),
                         WriteDeviceTopic::WRITE_TOPIC_INVALID) {}

/**
 * @brief Serializes the message into a string.
 * @return A string containing the contents of the message.
 */
string DummyMessage::serialize() { return "This is a dummy message"; }
} // namespace Messages
