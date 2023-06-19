// Project includes
#include <common.hpp>
#include <isx3_init_payload.hpp>
#include <isx3_is_conf_payload.hpp>
#include <isx3_payload_decoder.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <isx3_config_payload_generated.h>
#include <isx3_init_payload_generated.h>

using namespace Devices;

InitPayload *
Isx3PayloadDecoder::decodeInitPayload(const std::vector<unsigned char> &data,
                                      int magicNumber) {
  if (magicNumber == MAGIC_NUMBER_ISX3_INIT_PAYLOAD) {
    const Serialization::Devices::Isx3InitPayloadT *payload =
        Serialization::Devices::GetIsx3InitPayload(data.data())->UnPack();

    return new Isx3InitPayload(payload->ipAddress, payload->port);
  } else {
    return nullptr;
  }
}

ConfigurationPayload *
Isx3PayloadDecoder::decodeConfigPayload(const std::vector<unsigned char> &data,
                                        int magicNumber) {
  if (magicNumber == MAGIC_NUMBER_ISX3_IS_CONF_PAYLOAD) {
    const Serialization::Devices::Isx3IsConfPayloadT *payload =
        Serialization::Devices::GetIsx3IsConfPayload(data.data())->UnPack();

    // Transform the channel mapping.
    std::map<ChannelFunction, int> channelFunctionMapping;
    for (auto keyValuePair : payload->channelFunctionMapping) {
      channelFunctionMapping[static_cast<ChannelFunction>(
          keyValuePair.channelFunction())] = keyValuePair.channel();
    }

    return new Isx3IsConfPayload(payload->frequencyFrom, payload->frequencyTo,
                                 payload->measurementPoints,
                                 payload->repetitions, channelFunctionMapping,
                                 static_cast<IsScale>(payload->isScale),
                                 static_cast<MeasurmentConfigurationRange>(
                                     payload->measurementConfigurationRage),
                                 static_cast<MeasurmentConfigurationChannel>(
                                     payload->measurmentConfigurationChannel),
                                 static_cast<MeasurementConfiguration>(
                                     payload->measurementConfiguration),
                                 payload->precision, payload->amplitude);
  }

  else {
    return nullptr;
  }
}

ReadPayload *
Isx3PayloadDecoder::decodeReadPayload(const std::vector<unsigned char> &data,
                                      int magicNumber) {
  return nullptr;
}

WritePayload *
Isx3PayloadDecoder::decodeWritePayload(const std::vector<unsigned char> &data,
                                       int magicNumber) {
  return nullptr;
}