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
    : ConfigurationPayload(DeviceType::UNSPECIFIED),
      sentryWorkerMode(sentryWorkerMode), onTime(onTime), offTime(offTime) {}

vector<unsigned char> SentryConfigPayload::bytes() {

  Serialization::Workers::SentryConfigPayloadT intermediateObject;

  intermediateObject.offTime = this->offTime;
  intermediateObject.onTime = this->onTime;
  intermediateObject.sentryWorkerMode = this->sentryWorkerMode;

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::Workers::SentryConfigPayload::Pack(
      builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return vector<unsigned char>(buffer, buffer + builder.GetSize());
}

int SentryConfigPayload::getMagicNumber() {
  return MAGIC_NUMBER_SENTRY_CONF_PAYLOAD;
}