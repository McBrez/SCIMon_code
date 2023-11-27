// Project includes
#include <common.hpp>
#include <ob1_conf_payload.hpp>
#include <ob1_init_payload.hpp>
#include <ob1_payload_decoder.hpp>
#include <read_payload_ob1.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <ob1_config_payload_generated.h>
#include <ob1_init_payload_generated.h>
#include <ob1_read_payload_generated.h>

using namespace Devices;

InitPayload *
Ob1PayloadDecoder::decodeInitPayload(const std::vector<unsigned char> &data,
                                     int magicNumber) {
  const unsigned char *buffer = data.data();

  if (MAGIC_NUMBER_OB1_INIT_PAYLOAD == magicNumber) {
    const Serialization::Devices::Ob1InitPayloadT *ob1Payload =
        Serialization::Devices::GetOb1InitPayload(buffer)->UnPack();

    std::string deviceName = ob1Payload->deviceName;
    ChannelConfiguration channelConfiguration{
        ob1Payload->channelconfiguration->channel1(),
        ob1Payload->channelconfiguration->channel2(),
        ob1Payload->channelconfiguration->channel3(),
        ob1Payload->channelconfiguration->channel4()};

    return new Ob1InitPayload(deviceName, channelConfiguration);
  } else {
    return nullptr;
  }
}

ConfigurationPayload *
Ob1PayloadDecoder::decodeConfigPayload(const std::vector<unsigned char> &data,
                                       int magicNumber) {
  const unsigned char *buffer = data.data();

  if (MAGIC_NUMBER_OB1_CONF_PAYLOAD == magicNumber) {
    const Serialization::Devices::Ob1ConfPayloadT *ob1Payload =
        Serialization::Devices::GetOb1ConfPayload(buffer)->UnPack();

    ChannelPressures channelPressures =
        std::make_tuple(ob1Payload->pressureCh1, ob1Payload->pressureCh2,
                        ob1Payload->pressureCh3, ob1Payload->pressureCh4);

    return new Ob1ConfPayload(channelPressures);
  } else {
    return nullptr;
  }
}

ReadPayload *
Ob1PayloadDecoder::decodeReadPayload(const std::vector<unsigned char> &data,
                                     int magicNumber) {
  const unsigned char *buffer = data.data();

  if (MAGIC_NUMBER_OB1_READ_PAYLOAD == magicNumber) {
    const Serialization::Devices::Ob1ReadPayloadT *ob1Payload =
        Serialization::Devices::GetOb1ReadPayload(buffer)->UnPack();

    Ob1ChannelPressures channelPressures(ob1Payload->ob1Pressures->channel1(),
                                         ob1Payload->ob1Pressures->channel2(),
                                         ob1Payload->ob1Pressures->channel3(),
                                         ob1Payload->ob1Pressures->channel4());

    return new ReadPayloadOb1(channelPressures);
  } else {
    return nullptr;
  }

  return nullptr;
}

WritePayload *
Ob1PayloadDecoder::decodeWritePayload(const std::vector<unsigned char> &data,
                                      int magicNumber) {
  return nullptr;
}
