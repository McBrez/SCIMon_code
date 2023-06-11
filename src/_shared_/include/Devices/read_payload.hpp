#ifndef READ_PAYLOAD_HPP
#define READ_PAYLOAD_HPP

// Standard includes
#include <string>

// Project includes
#include <payload.hpp>

namespace Devices {

/**
 * @brief Base class for a read data package.
 */
class ReadPayload : public Payload {
public:
  /**
   * @brief Construct a new ReadPayload object.
   */
  ReadPayload();

  /**
   * @brief Destroy the ReadPayload object.
   */
  virtual ~ReadPayload() = 0;

  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual std::string serialize() = 0;
};

} // namespace Devices

#endif