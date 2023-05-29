// Project includes
#include <common.hpp>
#include <ob1_conf_payload.hpp>
#include <ob1_init_payload.hpp>
#include <ob1_payload_decoder.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <ob1_init_payload_generated.h>

using namespace Devices;

InitPayload *
Ob1PayloadDecoder::decodeInitPayload(const vector<unsigned char> &data,
                                     int magicNumber) {
  const unsigned char *buffer = data.data();

  if (MAGIC_NUMBER_OB1_INIT_PAYLOAD == magicNumber) {
    const Serialization::Devices::Ob1InitPayloadT *ob1Payload =
        Serialization::Devices::GetOb1InitPayload(buffer)->UnPack();

    string deviceName = ob1Payload->deviceName;
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
Ob1PayloadDecoder::decodeConfigPayload(const vector<unsigned char> &data,
                                       int magicNumber) {
  return nullptr;
}

ReadPayload *
Ob1PayloadDecoder::decodeReadPayload(const vector<unsigned char> &data,
                                     int magicNumber) {
  return nullptr;
}