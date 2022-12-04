#ifndef READ_PAYLOAD_OB1_HPP
#define READ_PAYLOAD_OB1_HPP

// Standard includes
#include <tuple>

// Project includes
#include <read_payload.hpp>

namespace Devices {

using Ob1ChannelPressures = tuple<double, double, double, double>;

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
  virtual string serialize() override;

  Ob1ChannelPressures getChannelPressures();

private:
  /// The channel pressures. In mBar,
  Ob1ChannelPressures channelPressures;
};
} // namespace Devices

#endif