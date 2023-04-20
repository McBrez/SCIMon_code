#ifndef HANDHSAKE_MESSAGE_HPP
#define HANDHSAKE_MESSAGE_HPP

// Project includes
#include <device_message.hpp>
#include <status_payload.hpp>

using namespace Devices;

namespace Messages {
/**
 * @brief Encapsulates a handshake message that is sent between network workers,
 * while they are establishing a connection.
 */
class HandshakeMessage : public DeviceMessage {
public:
  /**
   * @brief Construct a new Handshake Message object
   * @param source The source of the message.
   * @param destination The destination of the message.
   * @param statusPayloads The status payloads that shall be held by this
   * message.
   */
  HandshakeMessage(UserId source, UserId destination,
                   list<shared_ptr<StatusPayload>> statusPayloads);

  /**
   * @brief Destroy the Handshake Message object
   */
  virtual ~HandshakeMessage() override;

  /**
   * @brief Serializes the message into a human readable string.
   * @return A string representation of the message.
   */
  virtual string serialize() override;

  /**
   * @brief Serializes the message into a char vector.
   * @return Byte representation of the message.
   */
  virtual vector<unsigned char> bytes();

  /**
   * @brief Returns the status payloads held by this object.
   * @return Status payloads held by this object.
   */
  list<shared_ptr<StatusPayload>> getPayload();

private:
  /// The status payloads.
  list<shared_ptr<StatusPayload>> statusPayloads;
};
} // namespace Messages

#endif