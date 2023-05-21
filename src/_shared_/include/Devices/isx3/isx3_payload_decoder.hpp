#ifndef ISX3_PAYLOAD_DECODER_HPP
#define ISX3_PAYLOAD_DECODER_HPP

// Project includes
#include <payload_decoder.hpp>

namespace Devices {
class Isx3PayloadDecoder : public PayloadDecoder {
  virtual shared_ptr<InitPayload>
  decodeInitPayload(const vector<unsigned char> &data) override;

  virtual shared_ptr<ConfigurationPayload>
  decodeConfigPayload(const vector<unsigned char> &data) override;

  virtual shared_ptr<ReadPayload>
  decodeReadPayload(const vector<unsigned char> &data) override;
};
} // namespace Devices

#endif