#ifndef ISX3_ACK_PAYLOAD_HPP
#define ISX3_ACK_PAYLOAD_HPP

// Project inlcudes
#include <isx3_constants.hpp>
#include <read_payload.hpp>

namespace Devices {
/**
 * @brief Encapsulates an acknowledge message that has been received from an
 * ISX3 device.
 */
class Isx3AckPayload : public ReadPayload {
public:
  /**
   * @brief Construct a new Isx 3 Ack Payload object
   * @param ackType The acknowledgement type this object shall hold.
   */
  Isx3AckPayload(Isx3AckType ackType);

  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual string serialize() override;

  Isx3AckType getAckType();

private:
  /// The acknowledgement type.
  Isx3AckType ackType;
};
} // namespace Devices

#endif