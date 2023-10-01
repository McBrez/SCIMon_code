#ifndef REQUEST_KEY_PAYLOAD_HPP
#define REQUEST_KEY_PAYLOAD_HPP

// Project includes
#include <write_payload.hpp>

namespace Devices {

/**
 * @brief A payload that requests data from a message interface.
 */
class RequestKeyPayload : public WritePayload {
public:
  /**
   * @brief Construct a new RequestKeyPayload object.
   */
  RequestKeyPayload();

  /**
   * @brief Destroy the ReadPayload object.
   */
  virtual ~RequestKeyPayload() override;

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
};

} // namespace Devices

#endif
