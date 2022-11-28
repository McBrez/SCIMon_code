#ifndef READ_DEVICE_MESSAGE_HPP
#define READ_DEVICE_MESSAGE_HPP

// Project includes
#include <device_message.hpp>
#include <write_device_message.hpp>

namespace Messages {
/**
 * A message that has been sent by a device in response to a DeviceWriteMessage.
 */
class ReadDeviceMessage : public DeviceMessage {
public:
  ReadDeviceMessage(string data);
  string getData();
  virtual string serialize() override;

private:
  string data;
  /// Reference to the messsage this message is the response to.
  shared_ptr<WriteDeviceMessage> originalMessage;
};
} // namespace Messages

#endif