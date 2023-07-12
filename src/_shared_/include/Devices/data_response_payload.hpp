#ifndef DATA_RESPONE_PAYLOAD
#define DATA_RESPONE_PAYLOAD

// Project includes
#include <data_manager.hpp>
#include <read_payload.hpp>
#include <utilities.hpp>

using namespace Utilities;

namespace Devices {

/// The maximum count of elements, the response payload can hold.
#define SCIMON_RESPONSE_PAYLOAD_MAX_MESSAGE_LENGTH 1024

class DataResponsePayload : public ReadPayload {
public:
  /**
   * @brief Constructs response payloads from the given data vectors. The data
   * vectors are splitted into multiple response payloads, so that no payload
   * consists of more than SCIMON_RESPONSE_PAYLOAD_MAX_MESSAGE_LENGTH data
   * points.
   * @param from The start of the queried time frame.
   * @param to The end of the queried
   * @param key The key that has been queried
   * @param timestamps The timestamps that shall be held by the payload
   * @param values The values that shall be held by the payload.
   * @return Vector of pointers to the response payloads.
   */
  static std::vector<std::shared_ptr<DataResponsePayload>>
  constructDataResponsePayload(TimePoint from, TimePoint to, std::string key,
                               const std::vector<TimePoint> &timestamps,
                               const std::vector<Value> &values);

  /**
   * @brief Constructs a single data response payload. The timestamp/value
   * pairs, that are used in the creation of the repsonse payload are cut from
   * the passed vectors.
   * @param from The start of the queried time frame.
   * @param to The end of the queried
   * @param key The key that has been queried
   * @param timestamps The timestamps that shall be held by the payload
   * @param values The values that shall be held by the payload.
   * @return Vector of pointers to the response payloads.
   */
  static DataResponsePayload *constructSingleDataResponsePayload(
      TimePoint from, TimePoint to, std::string key,
      std::vector<TimePoint> &timestamps, std::vector<Value> &values);

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

  /// The start of the originally queried time frame.
  TimePoint from;
  /// The end of the originally queried time frame.
  TimePoint to;
  /// They key that has originally been queried
  std::string key;
  /// The count of data point, this message is holding.
  size_t count;
  /// Vector containing the timestamps.
  std::vector<TimePoint> timestamps;
  /// Vector containing the values.
  std::vector<Value> values;

private:
  /**
   * @brief Construct a new generic read payload with the given byte vector. The
   * constructor is hidden, in order to be able to ensure, that the payload does
   * not hold more than the defined count of elements. The object can be
   * constructed via constructDataResponsePayload().
   * @param byteVector The byte vector which shall be held by this payload.
   */
  DataResponsePayload(TimePoint from, TimePoint to, const std::string &key,
                      size_t count, const std::vector<TimePoint> &timestamps,
                      const std::vector<Value> &values);
};
} // namespace Devices

#endif
