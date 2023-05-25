#ifndef MESSAGE_FACTORY_HPP
#define MESSAGE_FACTORY_HPP

// Standard includes
#include <list>
#include <string>

// Project includes
#include <device_message.hpp>
#include <payload_decoder.hpp>
#include <read_payload.hpp>

// Generated includes
#include <device_message_generated.h>

using namespace Devices;

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
  vector<unsigned char> encodeMessage(shared_ptr<DeviceMessage> msg);

  /**
   * @brief Create an instance of the message factory, if there is not already
   * one existing.
   * @param payloadDecoders The payload decoders that the message factory shall
   * use, when decoding messages.
   * @return Pointer to a message factory instance. Null if an instance already
   * existed.
   */
  static MessageFactory *
  createInstace(list<shared_ptr<PayloadDecoder>> payloadDecoders);

  /**
   * @brief Gets the only instance of the class, assuming createInstance() has
   * already been called before.
   * @return Pointer to the only message factory instance. Null if
   * createInstance has not already been called.
   */
  static MessageFactory *getInstace();

  /**
   * @brief Returns the version of the scimon message interface implementation.
   * @return The version of the scimon message interface implementation.
   */
  string getVersion() const;

private:
  /**
   * @brief Checks if the given byte is an message type tag.
   * @param byte The byte that shall be analyzed.
   * @return TRUE if the given byte is a message type tag. FALSE otherwise.
   */
  bool isMessageTypeTag(char byte) const;

  vector<unsigned char> *extractFrame(list<unsigned char> &buffer);

  shared_ptr<DeviceMessage> decodeFrame(vector<unsigned char> *buffer,
                                        MessageType &msgTypeHint);

  shared_ptr<DeviceMessage> translateMessageContent(
      UserId sourceId, UserId destinationId,
      const Serialization::HandshakeMessageContentT *handshakeContent);

  shared_ptr<DeviceMessage> translateMessageContent(
      UserId sourceId, UserId destinationId,
      const Serialization::ReadDeviceMessageContentT *readDeviceContent);

  shared_ptr<DeviceMessage> translateMessageContent(
      UserId sourceId, UserId destinationId,
      const Serialization::InitDeviceMessageContentT *initDeviceContent);

  shared_ptr<DeviceMessage> translateMessageContent(
      UserId sourceId, UserId destinationId,
      const Serialization::ConfigDeviceMessageContentT *configDeviceContent);

  shared_ptr<DeviceMessage> translateMessageContent(
      UserId sourceId, UserId destinationId,
      const Serialization::WriteDeviceMessageContentT *writeDeviceContent);

  /**
   * @brief Construct a new Message Factory object
   */
  MessageFactory(list<shared_ptr<PayloadDecoder>> payloadDecoders);

  /// @brief The only instance of the class.
  static MessageFactory *instance;

  /// @brief List of payload decoders, that are available to the message
  /// factory.
  list<shared_ptr<PayloadDecoder>> payloadDecoders;
};

} // namespace Messages

#endif