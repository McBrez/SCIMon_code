#ifndef KEY_RESPONSE_PAYLOAD_HPP
#define KEY_RESPONSE_PAYLOAD_HPP

// Project includes
#include <data_manager.hpp>
#include <read_payload.hpp>

using namespace Utilities;

namespace Devices {

/**
 * @brief Payload that is a response to a RequestKeyPayload. Contains all the
 * data keys, the targeted object holds.
 */
class KeyResponsePayload : public ReadPayload {
public:
  /**
   * @brief Construct a new KeyResponsePayload object.
   * @param keyMapping The key mapping of the targeted object.
   * @param spectrumMapping The spectrum mapping of the targeted object.
   */
  KeyResponsePayload(const KeyMapping &keyMapping,
                     const SpectrumMapping &spectrumMapping);

  /**
   * @brief Destroy the ReadPayload object.
   */
  virtual ~KeyResponsePayload() override;

  /**
   * @brief Serializes the payload into a human readable string.
   * @return The payload in string representation.
   */
  virtual std::string serialize() override;

  /**
   * @brief Serializes the payload into bytes.
   * @return Byte vector, that depicts the payload.
   */
  virtual std::vector<unsigned char> bytes() override;

  /**
   * @brief Returns the magic number of the payload. This number is used to
   * identify the payload type when encoding and decoding payload. This number
   * has to be unique among all payload types.
   */
  virtual int getMagicNumber() override;

  /**
   * @brief Returns the held key mapping.
   * @return The held key mapping.
   */
  KeyMapping &getKeyMapping();

  /**
   * @brief Returns the held spectrum mapping.
   * @return The held spectrum mapping.
   */
  SpectrumMapping &getSpectrumMapping();

private:
  /// The key mapping of the targeted object.
  KeyMapping keyMapping;
  /// The spectrum mapping of the targeted object.
  SpectrumMapping spectrumMapping;
};

} // namespace Devices

#endif
