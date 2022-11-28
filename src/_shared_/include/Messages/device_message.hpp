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
private:
  string payload = "I am a message.";

public:
  /**
   * @brief Dummy function. This should be removed.
   */
  void getPayload();

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
};
} // namespace Messages

#endif