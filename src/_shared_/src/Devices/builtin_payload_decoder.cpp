// Project includes
#include <builtin_payload_decoder.hpp>
#include <generic_read_payload.hpp>

// Generated includes
#include <generic_read_payload_generated.h>
#include <is_payload_generated.h>

using namespace Devices;

shared_ptr<InitPayload>
BuiltinPayloadDecoder::decodeInitPayload(const vector<unsigned char> &data) {
  // No init payloads are built-in.
  return shared_ptr<InitPayload>();
}

shared_ptr<ConfigurationPayload>
BuiltinPayloadDecoder::decodeConfigPayload(const vector<unsigned char> &data) {
  // No config payloads are built-in.
  return shared_ptr<ConfigurationPayload>();
}

shared_ptr<ReadPayload>
BuiltinPayloadDecoder::decodeReadPayload(const vector<unsigned char> &data) {
  const unsigned char *buffer = data.data();

  const Serialization::GenericReadPayload *genericReadPayload =
      Serialization::GetGenericReadPayload(buffer);
  vector<unsigned char> byteVector(genericReadPayload->byteVector()->begin(),
                                   genericReadPayload->byteVector()->end());
  return shared_ptr<ReadPayload>(new GenericReadPayload(byteVector));
}