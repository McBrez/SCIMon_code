#ifndef DEVICE_MESSAGE_HPP
#define DEVICE_MESSAGE_HPP

// Standard includes
#include <memory>
#include <string>

using namespace std;

namespace Messages {

/// Forward declaration of the message interface. Can not include it here, as it
/// would lead to a circular dependency.
class MessageInterface;

/**
 * @brief Base class for messages that are sent or received from or by devices.
 */
class DeviceMessage {
public:
  /**
   * @brief Constructs the object.
   * @param source The message interface this message originates from.
   */
  DeviceMessage(shared_ptr<MessageInterface> source,
                shared_ptr<MessageInterface> destination);

  /**
   * @brief Destroy the Device Message object
   */
  virtual ~DeviceMessage() = 0;

  /**
   * @brief Serializes the message into a string.
   * @return A string containing the contents of the message.
   */
  virtual string serialize() = 0;

  /**
   * @brief Deserializes the given string into a message.
   *
   * @param data
   * @return true
   * @return false
   */
  static shared_ptr<DeviceMessage> deserialize(string data);

  /**
   * @brief Returns the unique id of this message.
   * @return The unique id of this message.
   */
  int getMessageId();

  /**
   * @brief Returns a reference to the message interface that created this
   * message.
   * @return A reference to the message interface that created this
   * message.
   */
  shared_ptr<MessageInterface> getSource();

  /**
   * @brief Returns a reference to the message interface that shall receive this
   * message.
   * @return A reference to the message interface that shall receive this
   * message.
   */
  shared_ptr<MessageInterface> getDestination();

private:
  // The unique id of the mssage.
  int messageId;

  /// The message interface this message originates from.
  shared_ptr<MessageInterface> source;

  /// The message interface this message shall be sent to.
  shared_ptr<MessageInterface> destination;

  /**
   * @brief Generates an unique id.
   * @return An unique id.
   */
  int generateId();
};
} // namespace Messages

#endif