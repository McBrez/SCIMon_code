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

SentryInitPayload::SentryInitPayload(
    std::shared_ptr<InitPayload> isInitPayload,
    std::shared_ptr<ConfigurationPayload> isConfigPayload,
    std::shared_ptr<InitPayload> pumpControllerInitPayload,
    std::shared_ptr<ConfigurationPayload> pumpControllerConfigPayload)
    : isSpecInitPayload(isInitPayload), isSpecConfPayload(isConfigPayload),
      pumpControllerInitPayload(pumpControllerInitPayload),
      pumpControllerConfigPayload(pumpControllerConfigPayload) {}

std::string SentryInitPayload::serialize() { return ""; }

std::vector<unsigned char> SentryInitPayload::bytes() {
  Serialization::Workers::SentryInitPayloadT intermediateObject;

  Serialization::Workers::NestedPayloadT *nestedPumpControlConfPayload =
      new Serialization::Workers::NestedPayloadT();
  nestedPumpControlConfPayload->magicNumber =
      this->pumpControllerConfigPayload->getMagicNumber();
  nestedPumpControlConfPayload->payload =
      this->pumpControllerConfigPayload->bytes();
  intermediateObject.pumpControllerConfPayload.reset(
      nestedPumpControlConfPayload);

  Serialization::Workers::NestedPayloadT *nestedPumpControlInitPayload =
      new Serialization::Workers::NestedPayloadT();
  nestedPumpControlInitPayload->magicNumber =
      this->pumpControllerInitPayload->getMagicNumber();
  nestedPumpControlInitPayload->payload =
      this->pumpControllerInitPayload->bytes();
  intermediateObject.pumpControllerInitPayload.reset(
      nestedPumpControlInitPayload);

  Serialization::Workers::NestedPayloadT *nestedIsInitPayload =
      new Serialization::Workers::NestedPayloadT();
  nestedIsInitPayload->magicNumber = this->isSpecInitPayload->getMagicNumber();
  nestedIsInitPayload->payload = this->isSpecInitPayload->bytes();
  intermediateObject.impedanceSpectrometerInitPayload.reset(
      nestedIsInitPayload);

  Serialization::Workers::NestedPayloadT *nestedIsConfigPayload =
      new Serialization::Workers::NestedPayloadT();
  nestedIsConfigPayload->magicNumber =
      this->isSpecConfPayload->getMagicNumber();
  nestedIsConfigPayload->payload = this->isSpecConfPayload->bytes();
  intermediateObject.impedanceSpectrometerConfPayload.reset(
      nestedIsConfigPayload);

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
