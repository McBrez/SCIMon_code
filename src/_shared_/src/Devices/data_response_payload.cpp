// Project includes
#include <common.hpp>
#include <data_response_payload.hpp>

// Generated includes.
#include <data_response_payload_generated.h>

using namespace Devices;
using namespace Utilities;

std::vector<std::shared_ptr<DataResponsePayload>>
DataResponsePayload::constructDataResponsePayload(
    TimePoint from, TimePoint to, std::string key,
    const std::vector<TimePoint> &timestamps,
    const std::vector<Value> &values) {

  std::vector<std::shared_ptr<DataResponsePayload>> retVal;
  bool atEnd = false;
  do {
    auto timestampsItBegin = timestamps.begin();
    auto timestampsItEnd =
        timestampsItBegin + SCIMON_RESPONSE_PAYLOAD_MAX_MESSAGE_LENGTH;

    auto valuesItBegin = values.begin();
    auto valuesItEnd =
        valuesItBegin + SCIMON_RESPONSE_PAYLOAD_MAX_MESSAGE_LENGTH;

    std::vector<TimePoint> timepointVec(timestampsItBegin, timestampsItEnd);
    std::vector<Value> valueVec(valuesItBegin, valuesItEnd);

    retVal.emplace_back(
        std::shared_ptr<DataResponsePayload>(new DataResponsePayload(
            from, to, key, timepointVec.size(), timepointVec, valueVec)));

    atEnd = timestampsItEnd == timestamps.end();

  } while (!atEnd);
  return retVal;
}

std::string DataResponsePayload::serialize() { return ""; }

std::vector<unsigned char> DataResponsePayload::DataResponsePayload::bytes() {

  return std::vector<unsigned char>();
}

int DataResponsePayload::getMagicNumber() {
  return MAGIC_NUMBER_DATA_RESPONSE_PAYLOAD;
}

DataResponsePayload::DataResponsePayload(
    TimePoint from, TimePoint to, const std::string &key, size_t count,
    const std::vector<TimePoint> &timestamps, const std::vector<Value> &values)
    : from(from), to(to), key(key), count(count), timestamps(timestamps),
      values(values) {}
