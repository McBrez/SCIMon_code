// Project includes
#include <isx3_init_payload.hpp>
#include <isx3_payload_decoder.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <isx3_init_payload_generated.h>

using namespace Devices;

shared_ptr<InitPayload>
Isx3PayloadDecoder::decodeInitPayload(const vector<unsigned char> &data) {
  const Serialization::Isx3InitPayloadT *payload =
      Serialization::GetIsx3InitPayload(data.data())->UnPack();

  return shared_ptr<InitPayload>(
      new Isx3InitPayload(payload->ipAddress, payload->port));
}

shared_ptr<ConfigurationPayload>
Isx3PayloadDecoder::decodeConfigPayload(const vector<unsigned char> &data) {
  return shared_ptr<ConfigurationPayload>();
}

shared_ptr<ReadPayload>
Isx3PayloadDecoder::decodeReadPayload(const vector<unsigned char> &data) {
  return shared_ptr<ReadPayload>();
}
