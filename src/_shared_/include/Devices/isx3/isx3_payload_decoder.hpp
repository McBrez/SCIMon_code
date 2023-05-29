#ifndef ISX3_PAYLOAD_DECODER_HPP
#define ISX3_PAYLOAD_DECODER_HPP

// Project includes
#include <payload_decoder.hpp>

namespace Devices {
class Isx3PayloadDecoder : public PayloadDecoder {
  virtual InitPayload *decodeInitPayload(const vector<unsigned char> &data,
                                         int magicNumber = 0) override;

  virtual ConfigurationPayload *
  decodeConfigPayload(const vector<unsigned char> &data,
                      int magicNumber = 0) override;

  virtual ReadPayload *decodeReadPayload(const vector<unsigned char> &data,
                                         int magicNumber = 0) override;
};
} // namespace Devices

#endif