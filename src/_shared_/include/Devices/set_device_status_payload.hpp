#ifndef SET_DEVICE_STATUS_PAYLOAD_HPP
#define SET_DEVICE_STATUS_PAYLOAD_HPP

// Project includes
#include <user_id.hpp>
#include <write_payload.hpp>

using namespace Messages;

namespace Devices {
/**
 * @brief Payload that tells an message interface to set the status of another
 * messsage interface object it manages.
 */
class SetDeviceStatusPayload : public WritePayload {
public:
  /**
   * @brief Construct a new ReadPayload object.
   */
  SetDeviceStatusPayload(UserId targetId, bool setStatus);

  /**
   * @brief Destroy the ReadPayload object.
   */
  virtual ~SetDeviceStatusPayload() override;

  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual std::string serialize() override;

  /**
   * @brief Serializes the payload into bytes.
   * @return Byte vector, that depicts the payload.
   */
  virtual std::vector<unsigned char> bytes() override;

  /**
   * @brief Returns the magic number of the payload. This number is used to
   * identify the payload type when encoding and decoding payload. This number
   * has to be unique among all payload types.
   */
  virtual int getMagicNumber() override;

  /// The id of the message interface, whose status shall be set.
  UserId targetId;
  /// The status that shall be set.
  bool setStatus;
};

} // namespace Devices

#endif