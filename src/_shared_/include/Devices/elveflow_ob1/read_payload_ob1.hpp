#ifndef READ_PAYLOAD_OB1_HPP
#define READ_PAYLOAD_OB1_HPP

// Standard includes
#include <tuple>

// Project includes
#include <read_payload.hpp>

namespace Devices {

using Ob1ChannelPressures = std::tuple<double, double, double, double>;

/**
 * @brief Encapsulates the device image of an OB1 device.
 */
class ReadPayloadOb1 : public ReadPayload {
public:
  ReadPayloadOb1(Ob1ChannelPressures channelPressures);
  /**
   * @brief Serializes the payload into a human readable string.
   *
   * @return The payload in string representation.
   */
  virtual std::string serialize() override;

  Ob1ChannelPressures getChannelPressures();

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

private:
  /// The channel pressures. In mBar,
  Ob1ChannelPressures channelPressures;
};
} // namespace Devices

#endif