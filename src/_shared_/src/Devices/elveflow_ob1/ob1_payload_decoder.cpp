// Project includes
#include <ob1_payload_decoder.hpp>

using namespace Devices;

shared_ptr<InitPayload>
Ob1PayloadDecoder::decodeInitPayload(const vector<unsigned char> &data) {
  return shared_ptr<InitPayload>();
}

shared_ptr<ConfigurationPayload>
Ob1PayloadDecoder::decodeConfigPayload(const vector<unsigned char> &data) {
  return shared_ptr<ConfigurationPayload>();
}

shared_ptr<ReadPayload>
Ob1PayloadDecoder::decodeReadPayload(const vector<unsigned char> &data) {
  return shared_ptr<ReadPayload>();
}