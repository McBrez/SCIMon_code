#ifndef REQUEST_DATA_PAYLOAD_HPP
#define REQUEST_DATA_PAYLOAD_HPP

// Project includes
#include <status_payload.hpp>
#include <utilities.hpp>
#include <write_payload.hpp>

using namespace Utilities;

namespace Devices {

/**
 * @brief Base class for a write data package.
 */
class RequestDataPayload : public WritePayload {
public:
  /**
   * @brief Construct a new ReadPayload object.
   */
  RequestDataPayload(DeviceType deviceType, TimePoint from, TimePoint to);

  /**
   * @brief Destroy the ReadPayload object.
   */
  virtual ~RequestDataPayload() override;

  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual string serialize() override;

  /**
   * @brief Serializes the payload into bytes.
   * @return Byte vector, that depicts the payload.
   */
  virtual vector<unsigned char> bytes() override;

  /**
   * @brief Returns the magic number of the payload. This number is used to
   * identify the payload type when encoding and decoding payload. This number
   * has to be unique among all payload types.
   */
  virtual int getMagicNumber() override;

  DeviceType deviceType;
  TimePoint from;
  TimePoint to;
};

} // namespace Devices

#endif