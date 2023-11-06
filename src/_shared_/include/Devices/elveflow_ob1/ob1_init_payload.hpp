#ifndef ob1_init_payload_HPP
#define ob1_init_payload_HPP

// Standard includes
#include <string>
#include <tuple>

// Project includes
#include <init_payload.hpp>

namespace Devices {

/// Typedef for a channelconfiguration. The integers of the tuple shall have the
/// values defined in ElveflowXX.h
using ChannelConfiguration = std::tuple<int, int, int, int>;

/**
 * @brief Encapsulated initialization data for an Elveflow OB1 device.
 */
class Ob1InitPayload : public InitPayload {
public:
  /**
   * @brief Construct the object.
   * @param deviceName The device name as reported by the NI MAX application.
   * @param channelConfig The channel configuration;
   */
  Ob1InitPayload(std::string deviceName, ChannelConfiguration channelConfig);
  /**
   * @brief Constructs the object.
   *
   * @param deviceName The device name as reported by the NI MAX application.
   * @param channel1Config The configuration of channel 1.
   * @param channel2Config The configuration of channel 2.
   * @param channel3Config The configuration of channel 3.
   * @param channel4Config The configuration of channel 4.
   */
  Ob1InitPayload(std::string deviceName, int channel1Config, int channel2Config,
                 int channel3Config, int channel4Config);

  /**
   * @brief Serializes the payload into a human readable string.
   *
   * @return The payload in string representation.
   */
  virtual std::string serialize() override;

  /**
   * @brief Returns the device name as string.
   *
   * @return The device name.
   */
  std::string getDeviceName();

  /**
   * @brief Returns the channel configuration.
   *
   * @return The channel configuration.
   */
  ChannelConfiguration getChannelConfig();

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
  /// The device name.
  std::string deviceName;
  /// The channel configuration
  ChannelConfiguration channelConfig;
};
} // namespace Devices

#endif
