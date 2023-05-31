// Project includes
#include <common.hpp>
#include <message_factory.hpp>
#include <sentry_init_payload.hpp>
#include <sentry_payload_decoder.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <sentry_config_payload_generated.h>
#include <sentry_init_payload_generated.h>

using namespace Devices;
using namespace Messages;
using namespace Workers;

InitPayload *
SentryPayloadDecoder::decodeInitPayload(const vector<unsigned char> &data,
                                        int magicNumber) {

  const unsigned char *buffer = data.data();

  if (MAGIC_NUMBER_SENTRY_INIT_PAYLOAD == magicNumber) {
    const Serialization::Workers::SentryInitPayloadT *sentryInitPayload =
        Serialization::Workers::GetSentryInitPayload(buffer)->UnPack();

    InitPayload *pumpControllerInitPayload =
        MessageFactory::getInstace()->decodeInitPayload(
            sentryInitPayload->pumpControllerInitPayload->payload,
            sentryInitPayload->pumpControllerInitPayload->magicNumber);
    ConfigurationPayload *pumpControllerConfigPayload =
        MessageFactory::getInstace()->decodeConfigurationPayload(
            sentryInitPayload->pumpControllerConfPayload->payload,
            sentryInitPayload->pumpControllerConfPayload->magicNumber);

    InitPayload *isInitPayload =
        MessageFactory::getInstace()->decodeInitPayload(
            sentryInitPayload->impedanceSpectrometerInitPayload->payload,
            sentryInitPayload->impedanceSpectrometerInitPayload->magicNumber);
    ConfigurationPayload *isConfigPayload =
        MessageFactory::getInstace()->decodeConfigurationPayload(
            sentryInitPayload->impedanceSpectrometerInitPayload->payload,
            sentryInitPayload->impedanceSpectrometerInitPayload->magicNumber);

    return new SentryInitPayload(isInitPayload, isConfigPayload,
                                 pumpControllerInitPayload,
                                 pumpControllerConfigPayload);
  } else {
    return nullptr;
  }
}

ConfigurationPayload *
SentryPayloadDecoder::decodeConfigPayload(const vector<unsigned char> &data,
                                          int magicNumber) {
  // No config payloads are built-in.
  return nullptr;
}

ReadPayload *
SentryPayloadDecoder::decodeReadPayload(const vector<unsigned char> &data,
                                        int magicNumber) {

  return nullptr;
}

WritePayload *
SentryPayloadDecoder::decodeWritePayload(const vector<unsigned char> &data,
                                         int magicNumber) {

  return nullptr;
}