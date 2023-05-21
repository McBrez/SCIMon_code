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
};
} // namespace Devices

#endif