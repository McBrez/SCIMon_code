// Project includes
#include <common.hpp>
#include <data_response_payload.hpp>

using namespace Devices;
using namespace Utilities;

std::vector<std::shared_ptr<DataResponsePayload>>
DataResponsePayload::constructDataResponsePayload(
    TimePoint from, TimePoint to, std::string key,
    const std::vector<TimePoint> &timestamps,
    const std::vector<Value> &values) {

  std::vector<std::shared_ptr<DataResponsePayload>> retVal;
  auto timestampsIt = timestamps.begin();

  std::swap_ranges;

  std::vector<> timestampsSlice

      retVal.emplace_back(shared_ptr<DataResponsePayload>(
          new DataResponsePayload(from, to, key)));
}

std::string DataResponsePayload::serialize() { return ""; }

std::vector<unsigned char> DataResponsePayload::DataResponsePayload::bytes() {}

int DataResponsePayload::getMagicNumber() {
  return MAGIC_NUMBER_DATA_RESPONSE_PAYLOAD;
}

DataResponsePayload::DataResponsePayload(
    TimePoint from, TimePoint to, const std::string &key, size_t count,
    const std::vector<TimePoint> &timestamps, const std::vector<Value> &values)
    : from(from), to(to), key(key), count(count), timestamps(timestamps),
      values(values) {}
