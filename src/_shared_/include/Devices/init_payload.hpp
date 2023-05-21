#ifndef INIT_PAYLOAD_HPP
#define INIT_PAYLOAD_HPP

// Standard includes
#include <string>

// Project includes
#include <payload.hpp>

using namespace std;

namespace Devices {
/**
 * @brief Base class for an initialization data package.
 */
class InitPayload : public Payload {
public:
  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual string serialize() = 0;
};
} // namespace Devices

#endif