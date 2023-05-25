#ifndef BUILTIN_PAYLOAD_DECODER_HPP
#define BUILTIN_PAYLOAD_DECODER_HPP

// Project includes
#include <payload_decoder.hpp>

namespace Devices {

/**
 * @brief A decoder for payloads that shall be supported by the message factory
 * per default.
 */
class BuiltinPayloadDecoder : public PayloadDecoder {
public:
  BuiltinPayloadDecoder() {}

  virtual InitPayload *
  decodeInitPayload(const vector<unsigned char> &data) override;

  virtual ConfigurationPayload *
  decodeConfigPayload(const vector<unsigned char> &data) override;

  virtual ReadPayload *
  decodeReadPayload(const vector<unsigned char> &data) override;
};
} // namespace Devices

#endif