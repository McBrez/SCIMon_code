// Project includes
#include <common.hpp>
#include <request_key_payload.hpp>

// Generated includes
#include <request_key_payload_generated.h>

namespace Devices {

RequestKeyPayload::RequestKeyPayload() {}

RequestKeyPayload::~RequestKeyPayload() {}

std::string RequestKeyPayload::serialize() { return ""; }

std::vector<unsigned char> RequestKeyPayload::bytes() {
  Serialization::Devices::RequestKeyPayloadT intermediateObject;
  intermediateObject.dummy = 0;

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::Devices::RequestKeyPayload::Pack(
      builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return std::vector<unsigned char>(buffer, buffer + builder.GetSize());
}

int RequestKeyPayload::getMagicNumber() {
  return MAGIC_NUMBER_REQUEST_KEY_PAYLOAD;
}

} // namespace Devices
