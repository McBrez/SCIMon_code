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

  virtual shared_ptr<InitPayload>
  decodeInitPayload(const vector<unsigned char> &data) override;

  virtual shared_ptr<ConfigurationPayload>
  decodeConfigPayload(const vector<unsigned char> &data) override;

  virtual shared_ptr<ReadPayload>
  decodeReadPayload(const vector<unsigned char> &data) override;
};
} // namespace Devices

#endif