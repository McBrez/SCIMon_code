// Project includes
#include <request_data_payload.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <request_data_payload_generated.h>

using namespace Devices;

RequestDataPayload::RequestDataPayload(TimePoint from, TimePoint to, const std::string &key)
    : key(key), from(from), to(to) {}

RequestDataPayload::~RequestDataPayload() {}

std::string RequestDataPayload::serialize() { return ""; }

std::vector<unsigned char> RequestDataPayload::bytes() {
  Serialization::Devices::RequestDataPayloadT intermediateObject;
  intermediateObject.from = this->from.time_since_epoch().count();
  intermediateObject.to = this->to.time_since_epoch().count();
  intermediateObject.key = this->key;

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::Devices::RequestDataPayload::Pack(
      builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return std::vector<unsigned char>(buffer, buffer + builder.GetSize());
}

int RequestDataPayload::getMagicNumber() {
  return MAGIC_NUMBER_REQUEST_DATA_PAYLOAD;
}
