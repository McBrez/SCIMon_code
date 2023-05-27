// Project includes
#include <builtin_payload_decoder.hpp>
#include <generic_read_payload.hpp>

// Generated includes
#include <generic_read_payload_generated.h>
#include <is_payload_generated.h>
#include <status_payload_generated.h>

using namespace Devices;

InitPayload *
BuiltinPayloadDecoder::decodeInitPayload(const vector<unsigned char> &data) {
  // No init payloads are built-in.
  return nullptr;
}

ConfigurationPayload *
BuiltinPayloadDecoder::decodeConfigPayload(const vector<unsigned char> &data) {
  // No config payloads are built-in.
  return nullptr;
}

ReadPayload *
BuiltinPayloadDecoder::decodeReadPayload(const vector<unsigned char> &data) {
  const unsigned char *buffer = data.data();

  // Try to parse it as generic payload.
  const Serialization::Devices::GenericReadPayload *genericReadPayload =
      Serialization::Devices::GetGenericReadPayload(buffer);
  vector<unsigned char> byteVector(genericReadPayload->byteVector()->begin(),
                                   genericReadPayload->byteVector()->end());
  // Did it work?

  return new GenericReadPayload(byteVector);
}