// Project includes
#include <common.hpp>
#include <sentry_config_payload.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <sentry_config_payload_generated.h>

using namespace Workers;

SentryConfigPayload::SentryConfigPayload(SentryWorkerMode sentryWorkerMode,
                                         Duration onTime, Duration offTime)
    : ConfigurationPayload(DeviceType::UNSPECIFIED, keyMapping),
      sentryWorkerMode(sentryWorkerMode), onTime(onTime), offTime(offTime) {}

std::vector<unsigned char> SentryConfigPayload::bytes() {

  Serialization::Workers::SentryConfigPayloadT intermediateObject;

  intermediateObject.offTime = this->offTime.count();
  intermediateObject.onTime = this->onTime.count();
  intermediateObject.sentryWorkerMode =
      static_cast<Serialization::Workers::SentryWorkerMode>(
          this->sentryWorkerMode);

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::Workers::SentryConfigPayload::Pack(
      builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return std::vector<unsigned char>(buffer, buffer + builder.GetSize());
}

int SentryConfigPayload::getMagicNumber() {
  return MAGIC_NUMBER_SENTRY_CONF_PAYLOAD;
}
