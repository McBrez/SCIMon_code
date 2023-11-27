// Project includes
#include <common.hpp>
#include <ob1_conf_payload.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <ob1_config_payload_generated.h>

namespace Devices {

Ob1ConfPayload::Ob1ConfPayload(ChannelPressures channelPressures)
    : ConfigurationPayload(DeviceType::PUMP_CONTROLLER),
      channelPressures(channelPressures) {}

Ob1ConfPayload::~Ob1ConfPayload() {}

int Ob1ConfPayload::getMagicNumber() { return MAGIC_NUMBER_OB1_CONF_PAYLOAD; }

std::vector<unsigned char> Ob1ConfPayload::bytes() {
  Serialization::Devices::Ob1ConfPayloadT intermediateObject;

  intermediateObject.pressureCh1 = std::get<0>(this->channelPressures);
  intermediateObject.pressureCh2 = std::get<1>(this->channelPressures);
  intermediateObject.pressureCh3 = std::get<2>(this->channelPressures);
  intermediateObject.pressureCh4 = std::get<3>(this->channelPressures);

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::Devices::Ob1ConfPayload::Pack(
      builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return std::vector<unsigned char>(buffer, buffer + builder.GetSize());
}

ChannelPressures Ob1ConfPayload::getChannelPressures() {
  return this->channelPressures;
}

} // namespace Devices
