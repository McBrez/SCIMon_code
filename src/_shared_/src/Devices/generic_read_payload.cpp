// Project includes
#include <generic_read_payload.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <generic_read_payload_generated.h>

using namespace Devices;

GenericReadPayload::GenericReadPayload(vector<unsigned char> byteVector)
    : byteVector(byteVector) {}

string GenericReadPayload::serialize() { return ""; }

vector<unsigned char> GenericReadPayload::bytes() {
  Serialization::GenericReadPayloadT intermediateObject;
  intermediateObject.byteVector = this->byteVector;

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(
      Serialization::GenericReadPayload::Pack(builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return vector<unsigned char>(buffer, buffer + builder.GetSize());
}

vector<unsigned char> GenericReadPayload::getByteVector() {
  return this->byteVector;
}
