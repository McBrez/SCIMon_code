#ifndef MESSAGE_FACTORY_HPP
#define MESSAGE_FACTORY_HPP

// Project includes
#include <device_message.hpp>

namespace Messages {

/**
 * @brief Identifies the top level message types and their identifier byte.
 */
enum MessageType {
  INVALID_MESSAGE_TYPE = 0x00,

  WRITE_DEVICE_MESSAGE = 0x01,

  READ_DEVICE_MESSAGE = 0x02,

  HANDSHAKE_MESSAGE = 0x03,

  INIT_DEVICE_MESSAGE = 0x04,

  CONFIG_DEVICE_MESSAGE = 0x05
};

/**
 * @brief Identifies the payload types and their identifier byte.
 */
enum PayloadType { IS_PAYLOAD = 0x01 };

/**
 * @brief Factory class, that enables convenient creation, encoding and decoding
 * of messages.
 */
class MessageFactory {
public:
  /**
   * @brief Construct a new Message Factory object
   */
  MessageFactory();

  /**
   * @brief Decodes a serialized message into a device message object.
   * @param buffer The buffer that contains the serialized message. If a message
   * has been detected in the buffer and has been successfully decoded, the
   * corresponding bytes are removed from the buffer.
   * @return Pointer to a device message if decoding was successfull. Null
   * pointer otherwise.
   */
  shared_ptr<DeviceMessage> decodeMessage(vector<unsigned char> &buffer);

  /**
   * @brief Encodes the given message into a byte vector.
   * @param msg The message that shall be ecnoded.
   * @return The encoded message.
   */
  static vector<unsigned char> encodeMessage(shared_ptr<DeviceMessage> msg);

private:
  /**
   * @brief Checks if the given byte is an message type tag.
   * @param byte The byte that shall be analyzed.
   * @return TRUE if the given byte is a message type tag. FALSE otherwise.
   */
  bool isMessageTypeTag(char byte) const;

  vector<unsigned char> extractFrame(list<unsigned char> &buffer);

  shared_ptr<DeviceMessage> decodeFrame(vector<unsigned char> &buffer,
                                        MessageType &msgTypeHint);

  vector<unsigned char> encodeReadPayload(shared_ptr<ReadPayload> readPayload);

  shared_ptr<ReadPayload>
  decodeReadPayload(const vector<unsigned char> &buffer);
};

} // namespace Messages

#endif