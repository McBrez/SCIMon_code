#ifndef WRITE_PAYLOAD_HPP
#define WRITE_PAYLOAD_HPP

// Standard includes
#include <string>

// Project includes
#include <payload.hpp>

using namespace std;

namespace Devices {

/**
 * @brief Base class for a write data package.
 */
class WritePayload : public Payload {
public:
  /**
   * @brief Construct a new ReadPayload object.
   */
  WritePayload(){};

  /**
   * @brief Destroy the ReadPayload object.
   */
  virtual ~WritePayload() = 0;

  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual string serialize() = 0;
};

} // namespace Devices

#endif