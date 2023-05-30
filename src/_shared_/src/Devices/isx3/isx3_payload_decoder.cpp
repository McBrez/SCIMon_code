// Project includes
#include <isx3_init_payload.hpp>
#include <isx3_payload_decoder.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <isx3_init_payload_generated.h>

using namespace Devices;

InitPayload *
Isx3PayloadDecoder::decodeInitPayload(const vector<unsigned char> &data,
                                      int magicNumber) {
  const Serialization::Devices::Isx3InitPayloadT *payload =
      Serialization::Devices::GetIsx3InitPayload(data.data())->UnPack();

  return new Isx3InitPayload(payload->ipAddress, payload->port);
}

ConfigurationPayload *
Isx3PayloadDecoder::decodeConfigPayload(const vector<unsigned char> &data,
                                        int magicNumber) {
  return nullptr;
}

ReadPayload *
Isx3PayloadDecoder::decodeReadPayload(const vector<unsigned char> &data,
                                      int magicNumber) {
  return nullptr;
}

WritePayload *
Isx3PayloadDecoder::decodeWritePayload(const vector<unsigned char> &data,
                                       int magicNumber) {
  return nullptr;
}