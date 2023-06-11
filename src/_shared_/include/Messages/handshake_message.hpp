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
   * @param version The version of the message lib of the host that generated
   * this message. message.
   */
  HandshakeMessage(UserId source, UserId destination,
                   std::list<std::shared_ptr<StatusPayload>> statusPayloads,
                   std::string version);

  /**
   * @brief Destroy the Handshake Message object
   */
  virtual ~HandshakeMessage() override;

  /**
   * @brief Serializes the message into a human readable string.
   * @return A string representation of the message.
   */
  virtual std::string serialize() override;

  /**
   * @brief Serializes the message into a char vector.
   * @return Byte representation of the message.
   */
  virtual std::vector<unsigned char> bytes();

  /**
   * @brief Returns the status payloads held by this object.
   * @return Status payloads held by this object.
   */
  std::list<std::shared_ptr<StatusPayload>> getPayload();

  /**
   * @brief Returns the message lib version string of the host that generated
   * this message.
   * @return The message lib version string of the host that generated
   * this message.
   */
  std::string getVersion();

private:
  /// The status payloads.
  std::list<std::shared_ptr<StatusPayload>> statusPayloads;

  /// The version of the message lib of the host that generated this message.
  std::string version;
};
} // namespace Messages

#endif