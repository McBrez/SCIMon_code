#ifndef READ_PAYLOAD_HPP
#define READ_PAYLOAD_HPP

// Standard includes
#include <string>

using namespace std;

namespace Devices {

/**
 * @brief Base class for a read data package.
 */
class ReadPayload {
public:
  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual string serialize() = 0;
};

} // namespace Devices

#endif