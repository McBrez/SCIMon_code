#ifndef SENTRY_PAYLOAD_DECODER_HPP
#define SENTRY_PAYLOAD_DECODER_HPP

// Project includes
#include <payload_decoder.hpp>

namespace Devices {

/**
 * @brief A decoder for payloads that shall be supported by the message factory
 * per default.
 */
class SentryPayloadDecoder : public PayloadDecoder {
public:
  SentryPayloadDecoder() {}

  virtual InitPayload *decodeInitPayload(const std::vector<unsigned char> &data,
                                         int magicNumber = 0) override;

  virtual ConfigurationPayload *
  decodeConfigPayload(const std::vector<unsigned char> &data,
                      int magicNumber = 0) override;

  virtual ReadPayload *decodeReadPayload(const std::vector<unsigned char> &data,
                                         int magicNumber = 0) override;

  virtual WritePayload *
  decodeWritePayload(const std::vector<unsigned char> &data,
                     int magicNumber = 0) override;
};
} // namespace Devices

#endif