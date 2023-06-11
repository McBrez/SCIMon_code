#ifndef GENERIC_READ_PAYLOAD_HPP
#define GENERIC_READ_PAYLOAD_HPP

// Standard includes
#include <vector>

// Project includes
#include <read_payload.hpp>

namespace Devices {
/**
 * @brief Encapsulates a read payload that may hold a byte vector.
 */
class GenericReadPayload : public ReadPayload {
public:
  /**
   * @brief Construct a new generic read payload with the given byte vector.
   * @param byteVector The byte vector which shall be held by this payload.
   */
  GenericReadPayload(std::vector<unsigned char> byteVector);

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
   * @brief Returns the byte vector held by this payload.
   * @return vector<unsigned int>
   */
  std::vector<unsigned char> getByteVector();

  /**
   * @brief Returns the magic number of the payload. This number is used to
   * identify the payload type when encoding and decoding payload. This number
   * has to be unique among all payload types.
   */
  virtual int getMagicNumber() override;

private:
  /// @brief The byte vector that is held by the payload.
  std::vector<unsigned char> byteVector;
};
} // namespace Devices

#endif