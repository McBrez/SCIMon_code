// Project includes
#include <ob1_init_payload.hpp>

namespace Devices {

Ob1InitPayload::Ob1InitPayload(string deviceName,
                               ChannelConfiguration channelConfig)
    : deviceName(deviceName), channelConfig(channelConfig) {}

Ob1InitPayload::Ob1InitPayload(string deviceName, int channel1Config,
                               int channel2Config, int channel3Config,
                               int channel4Config)
    : deviceName(deviceName),
      channelConfig(std::make_tuple(channel1Config, channel2Config,
                                    channel3Config, channel4Config)) {}

string Ob1InitPayload::serialize() { return ""; }

string Ob1InitPayload::getDeviceName() { return this->deviceName; }

ChannelConfiguration Ob1InitPayload::getChannelConfig() {
  return this->channelConfig;
}

} // namespace Devices