// Project includes
#include <ob1_init_payload.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <ob1_init_payload_generated.h>

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

vector<unsigned char> Ob1InitPayload::bytes() {
  Serialization::Devices::Ob1InitPayloadT intermediateObject;
  intermediateObject.channelconfiguration.reset(
      new Serialization::Devices::ChannelConfiguration(
          get<0>(this->channelConfig), get<1>(this->channelConfig),
          get<2>(this->channelConfig), get<3>(this->channelConfig)));
  intermediateObject.deviceName = this->deviceName;

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(
      Serialization::Devices::Ob1InitPayload::Pack(builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return vector<unsigned char>(buffer, buffer + builder.GetSize());
}

} // namespace Devices