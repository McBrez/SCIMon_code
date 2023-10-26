// Project includes
#include <common.hpp>
#include <isx3_init_payload.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <isx3_init_payload_generated.h>

namespace Devices {

Isx3InitPayload::Isx3InitPayload(int comPort) : comPort(comPort) {}

std::string Isx3InitPayload::serialize() { return ""; }

int Isx3InitPayload::getComPort() const { return this->comPort; }

std::vector<unsigned char> Isx3InitPayload::bytes() {
  Serialization::Devices::Isx3InitPayloadT intermediateObject;
  intermediateObject.comPort = this->comPort;

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::Devices::Isx3InitPayload::Pack(
      builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return std::vector<unsigned char>(buffer, buffer + builder.GetSize());
}

int Isx3InitPayload::getMagicNumber() { return MAGIC_NUMBER_ISX3_INIT_PAYLOAD; }

} // namespace Devices
