#ifndef PAYLOAD_HPP
#define PAYLOAD_HPP

// Standard includes
#include <vector>

using namespace std;

namespace Devices {
/**
 * @brief A generic payload that may be used for device initialization,
 * configuration or data exchange.
 */
class Payload {
public:
  /**
   * @brief Serializes the payload into bytes.
   * @return Byte vector, that depicts the payload.
   */
  virtual vector<unsigned char> bytes() = 0;

  /**
   * @brief Returns the magic number of the payload. This number is used to
   * identify the payload type when encoding and decoding payload. This number
   * has to be unique among all payload types.
   */
  virtual int getMagicNumber() = 0;
};
} // namespace Devices

#endif