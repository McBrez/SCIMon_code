#ifndef DEVICE_MESSAGE_HPP
#define DEVICE_MESSAGE_HPP

// Standard includes
#include <memory>
#include <string>

using namespace std;

namespace Messages {
/**
 * @brief Base class for messages that are sent or received from or by devices.
 */
class DeviceMessage {
public:
  /**
   * @brief Constructs the object.
   *
   */
  DeviceMessage();

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

private:
  // The unique id of the mssage.
  int messageId;

  /**
   * @brief Generates an unique id.
   * @return An unique id.
   */
  int generateId();
};
} // namespace Messages

#endif