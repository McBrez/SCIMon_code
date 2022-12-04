// Project includes
#include <init_payload_ob1.hpp>

namespace Devices {

InitPayloadOb1::InitPayloadOb1(string deviceName,
                               ChannelConfiguration channelConfig)
    : deviceName(deviceName), channelConfig(channelConfig) {}

InitPayloadOb1::InitPayloadOb1(string deviceName, int channel1Config,
                               int channel2Config, int channel3Config,
                               int channel4Config)
    : deviceName(deviceName),
      channelConfig(std::make_tuple(channel1Config, channel2Config,
                                    channel3Config, channel4Config)) {}

string InitPayloadOb1::serialize() { return ""; }

string InitPayloadOb1::getDeviceName() { return this->deviceName; }

ChannelConfiguration InitPayloadOb1::getChannelConfig() {
  return this->channelConfig;
}

} // namespace Devices