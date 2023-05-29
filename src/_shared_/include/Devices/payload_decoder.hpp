#ifndef PAYLOAD_DECODER_HPP
#define PAYLOAD_DECODER_HPP

// Standard includes
#include <memory>
#include <vector>

// Project includes
#include <configuration_payload.hpp>
#include <init_payload.hpp>
#include <read_payload.hpp>

namespace Devices {
/**
 * @brief Superclass of a payload decoder. Decodes a byte array into a device
 * specific payload. Each device has to implement its payload decoder.
 */
class PayloadDecoder {
public:
  PayloadDecoder() {}

  virtual InitPayload *decodeInitPayload(const vector<unsigned char> &data,
                                         int magicNumber = 0) = 0;

  virtual ConfigurationPayload *
  decodeConfigPayload(const vector<unsigned char> &data,
                      int magicNumber = 0) = 0;

  virtual ReadPayload *decodeReadPayload(const vector<unsigned char> &data,
                                         int magicNumber = 0) = 0;
};
} // namespace Devices

#endif