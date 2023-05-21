#ifndef OB1_PAYLOAD_DECODER_HPP
#define OB1_PAYLOAD_DECODER_HPP

// Project includes
#include <payload_decoder.hpp>

namespace Devices {
class Ob1PayloadDecoder : public PayloadDecoder {
  virtual shared_ptr<InitPayload>
  decodeInitPayload(const vector<unsigned char> &data) override;

  virtual shared_ptr<ConfigurationPayload>
  decodeConfigPayload(const vector<unsigned char> &data) override;

  virtual shared_ptr<ReadPayload>
  decodeReadPayload(const vector<unsigned char> &data) override;
};
} // namespace Devices

#endif