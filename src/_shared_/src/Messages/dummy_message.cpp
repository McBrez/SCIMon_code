// Project includes
#include <dummy_device.hpp>
#include <dummy_message.hpp>

namespace Messages {
DummyMessage::DummyMessage()
    : WriteDeviceMessage(shared_ptr<MessageInterface>(),
                         shared_ptr<MessageInterface>(),
                         WriteDeviceTopic::WRITE_TOPIC_INVALID) {
}

/**
 * @brief Serializes the message into a string.
 * @return A string containing the contents of the message.
 */
string DummyMessage::serialize() { return "This is a dummy message"; }
} // namespace Messages
