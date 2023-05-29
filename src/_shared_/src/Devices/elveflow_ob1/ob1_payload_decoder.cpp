// Project includes
#include <ob1_payload_decoder.hpp>

using namespace Devices;

InitPayload *
Ob1PayloadDecoder::decodeInitPayload(const vector<unsigned char> &data,
                                     int magicNumber) {
  return nullptr;
}

ConfigurationPayload *
Ob1PayloadDecoder::decodeConfigPayload(const vector<unsigned char> &data,
                                       int magicNumber) {
  return nullptr;
}

ReadPayload *
Ob1PayloadDecoder::decodeReadPayload(const vector<unsigned char> &data,
                                     int magicNumber) {
  return nullptr;
}