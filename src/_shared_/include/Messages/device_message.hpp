#ifndef DEVICE_MESSAGE_HPP
#define DEVICE_MESSAGE_HPP

// Project includes
#include <user_id.hpp>

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
   * @param source The id of the object this message originates from.
   * @param destination The id of the object this message shall be sent to.
   */
  DeviceMessage(UserId source, UserId destination);

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
   * @brief Returns the id of the source of this message.
   * @return The id of the source of this message.
   */
  UserId getSource();

  /**
   * @brief Returns the id of the destination of this message.
   * @return The id of the destination of this message.
   */
  UserId getDestination();

protected:
  void setSource(UserId source);
  void setDestination(UserId destination);

private:
  // The unique id of the mssage.
  int messageId;

  /// The id of the object this message originates from.
  UserId source;

  /// The id of the object this message shall be sent to.
  UserId destination;

  /**
   * @brief Generates an unique id.
   * @return An unique id.
   */
  int generateId();
};
} // namespace Messages

#endif