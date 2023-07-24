// Project includes
#include <common.hpp>
#include <ob1_conf_payload.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <ob1_config_payload_generated.h>

namespace Devices {

Ob1ConfPayload::Ob1ConfPayload()
    : ConfigurationPayload(DeviceType::PUMP_CONTROLLER) {}

Ob1ConfPayload::~Ob1ConfPayload() {}

int Ob1ConfPayload::getMagicNumber() { return MAGIC_NUMBER_OB1_CONF_PAYLOAD; }

std::vector<unsigned char> Ob1ConfPayload::bytes() {
  Serialization::Devices::Ob1ConfPayloadT intermediateObject;

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::Devices::Ob1ConfPayload::Pack(
      builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return std::vector<unsigned char>(buffer, buffer + builder.GetSize());
}

} // namespace Devices
