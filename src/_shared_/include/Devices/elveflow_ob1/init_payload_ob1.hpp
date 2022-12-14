#ifndef INIT_PAYLOAD_OB1_HPP
#define INIT_PAYLOAD_OB1_HPP

// Standard includes
#include <string>
#include <tuple>

// Project includes
#include <init_payload.hpp>

using namespace std;

namespace Devices {

/// Typedef for a channelconfiguration. The integers of the tuple shall have the
/// values defind in ElveflowXX.h
using ChannelConfiguration = tuple<int, int, int, int>;

/**
 * @brief Encapsulated initialization data for an Elveflow OB1 device.
 */
class InitPayloadOb1 : public InitPayload {
public:
  /**
   * @brief Construct the object.
   * @param deviceName The device name as reported by the NI MAX application.
   * @param channelConfig The channel configuration;
   */
  InitPayloadOb1(string deviceName, ChannelConfiguration channelConfig);
  /**
   * @brief Constructs the object.
   *
   * @param deviceName The device name as reported by the NI MAX application.
   * @param channel1Config The configuration of channel 1.
   * @param channel2Config The configuration of channel 2.
   * @param channel3Config The configuration of channel 3.
   * @param channel4Config The configuration of channel 4.
   */
  InitPayloadOb1(string deviceName, int channel1Config, int channel2Config,
                 int channel3Config, int channel4Config);

  /**
   * @brief Serializes the payload into a human readable string.
   *
   * @return The payload in string representation.
   */
  virtual string serialize() override;

  /**
   * @brief Returns the device name as string.
   *
   * @return The device name.
   */
  string getDeviceName();

  /**
   * @brief Returns the channel configuration.
   *
   * @return The channel configuration.
   */
  ChannelConfiguration getChannelConfig();

private:
  /// The device name.
  string deviceName;
  /// The channel configuration
  ChannelConfiguration channelConfig;
};
} // namespace Devices

#endif