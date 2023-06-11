// Project includes
#include <common.hpp>
#include <generic_read_payload.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <generic_read_payload_generated.h>

using namespace Devices;

GenericReadPayload::GenericReadPayload(std::vector<unsigned char> byteVector)
    : byteVector(byteVector) {}

std::string GenericReadPayload::serialize() { return ""; }

std::vector<unsigned char> GenericReadPayload::bytes() {
  Serialization::Devices::GenericReadPayloadT intermediateObject;
  intermediateObject.byteVector = this->byteVector;

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::Devices::GenericReadPayload::Pack(
      builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return std::vector<unsigned char>(buffer, buffer + builder.GetSize());
}

std::vector<unsigned char> GenericReadPayload::getByteVector() {
  return this->byteVector;
}

int GenericReadPayload::getMagicNumber() {
  return MAGIC_NUMBER_GENERIC_READ_PAYLOAD;
}