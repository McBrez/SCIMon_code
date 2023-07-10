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
   * @param from The start of the queried timeframe.
   * @param to THe end of the queried timeframe.
   * @param key The key that shall be queried.
   */
    RequestDataPayload(TimePoint from, TimePoint to, const std::string &key);

  /**
   * @brief Destroy the ReadPayload object.
   */
  virtual ~RequestDataPayload() override;

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

  /// The start of the queried time frame.
  TimePoint from;
  /// THe end of the queried time frame.
  TimePoint to;
  /// The key which is queried.
  std::string key;
};

} // namespace Devices

#endif
