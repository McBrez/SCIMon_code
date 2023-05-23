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

  virtual shared_ptr<InitPayload>
  decodeInitPayload(const vector<unsigned char> &data) = 0;

  virtual shared_ptr<ConfigurationPayload>
  decodeConfigPayload(const vector<unsigned char> &data) = 0;

  virtual shared_ptr<ReadPayload>
  decodeReadPayload(const vector<unsigned char> &data) = 0;
};
} // namespace Devices

#endif