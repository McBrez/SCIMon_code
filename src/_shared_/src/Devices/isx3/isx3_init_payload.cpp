// Project includes
#include <common.hpp>
#include <isx3_init_payload.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <isx3_init_payload_generated.h>

namespace Devices {

Isx3InitPayload::Isx3InitPayload(std::string ipAddress, int port)
    : ipAddress(ipAddress), port(port) {}

std::string Isx3InitPayload::serialize() { return ""; }

std::string Isx3InitPayload::getIpAddress() { return this->ipAddress; }

int Isx3InitPayload::getPort() { return this->port; }

std::vector<unsigned char> Isx3InitPayload::bytes() {
  Serialization::Devices::Isx3InitPayloadT intermediateObject;
  intermediateObject.ipAddress = this->ipAddress;
  intermediateObject.port = this->port;

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::Devices::Isx3InitPayload::Pack(
      builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return std::vector<unsigned char>(buffer, buffer + builder.GetSize());
}

int Isx3InitPayload::getMagicNumber() { return MAGIC_NUMBER_ISX3_INIT_PAYLOAD; }

} // namespace Devices
