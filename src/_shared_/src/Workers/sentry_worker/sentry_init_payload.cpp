// Project includes
#include <sentry_init_payload.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <sentry_init_payload_generated.h>

namespace Workers {

SentryInitPayload::SentryInitPayload(
    InitPayload *isInitPayload, ConfigurationPayload *isConfigPayload,
    InitPayload *pumpControllerInitPayload,
    ConfigurationPayload *pumpControllerConfigPayload)
    : isSpecInitPayload(isInitPayload), isSpecConfPayload(isConfigPayload),
      pumpControllerInitPayload(pumpControllerInitPayload),
      pumpControllerConfigPayload(pumpControllerConfigPayload) {}

std::string SentryInitPayload::serialize() { return ""; }

std::vector<unsigned char> SentryInitPayload::bytes() {
  Serialization::Workers::SentryInitPayloadT intermediateObject;

  intermediateObject.pumpControllerConfPayload->magicNumber =
      this->pumpControllerConfigPayload->getMagicNumber();
  intermediateObject.pumpControllerConfPayload->payload =
      this->pumpControllerConfigPayload->bytes();

  intermediateObject.pumpControllerInitPayload->magicNumber =
      this->pumpControllerInitPayload->getMagicNumber();
  intermediateObject.pumpControllerInitPayload->payload =
      this->pumpControllerInitPayload->bytes();

  intermediateObject.impedanceSpectrometerConfPayload->magicNumber =
      this->isSpecConfPayload->getMagicNumber();
  intermediateObject.impedanceSpectrometerConfPayload->payload =
      this->isSpecConfPayload->bytes();

  intermediateObject.impedanceSpectrometerInitPayload->magicNumber =
      this->isSpecInitPayload->getMagicNumber();
  intermediateObject.impedanceSpectrometerInitPayload->payload =
      this->isSpecInitPayload->bytes();

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::Workers::SentryInitPayload::Pack(
      builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return std::vector<unsigned char>(buffer, buffer + builder.GetSize());
}

int SentryInitPayload::getMagicNumber() {
  return MAGIC_NUMBER_SENTRY_INIT_PAYLOAD;
}

} // namespace Workers
