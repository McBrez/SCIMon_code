// Project includes
#include <common.hpp>
#include <isx3_is_conf_payload.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <isx3_config_payload_generated.h>

namespace Devices {

Isx3IsConfPayload::Isx3IsConfPayload(
    double frequencyFrom, double frequencyTo, int measurementPoints,
    int repetitions, std::map<ChannelFunction, int> channel, IsScale scale,
    MeasurmentConfigurationRange measurementConfRange,
    MeasurmentConfigurationChannel measurementConfChannel,
    MeasurementConfiguration measurementConfiguration, const double precision,
    const double amplitude)
    : IsConfiguration(frequencyFrom, frequencyTo, measurementPoints,
                      repetitions),
      channel(channel), scale(scale),
      measurementConfRange(measurementConfRange),
      measurementConfChannel(measurementConfChannel),
      measurementConfiguration(measurementConfiguration), precision(precision),
      amplitude(amplitude) {}

Isx3IsConfPayload::~Isx3IsConfPayload() {}

std::vector<unsigned char> Isx3IsConfPayload::bytes() {
  Serialization::Devices::Isx3IsConfPayloadT intermediateObject;
  intermediateObject.frequencyFrom = this->frequencyFrom;
  intermediateObject.frequencyTo = this->frequencyTo;
  intermediateObject.measurementPoints = this->measurementPoints;
  intermediateObject.repetitions = this->repetitions;

  // Pare the channel function mapping into a vector.
  std::vector<Serialization::Devices::ChannelFunction>
      channelFunctionMappingVect;
  for (auto keyValuePair : this->channel) {
    channelFunctionMappingVect.push_back(
        Serialization::Devices::ChannelFunction(
            static_cast<int>(keyValuePair.first), keyValuePair.second));
  }
  intermediateObject.channelFunctionMapping = channelFunctionMappingVect;

  intermediateObject.isScale =
      static_cast<Serialization::Devices::IsScale>(this->scale);
  intermediateObject.measurementConfigurationRage =
      static_cast<Serialization::Devices::MeasurmentConfigurationRange>(
          this->measurementConfRange);
  intermediateObject.measurmentConfigurationChannel =
      static_cast<Serialization::Devices::MeasurmentConfigurationChannel>(
          this->measurementConfChannel);
  intermediateObject.measurementConfiguration =
      static_cast<Serialization::Devices::MeasurementConfiguration>(
          this->measurementConfiguration);
  intermediateObject.precision = this->precision;
  intermediateObject.amplitude = this->amplitude;

  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::Devices::Isx3IsConfPayload::Pack(
      builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();

  return std::vector<unsigned char>(buffer, buffer + builder.GetSize());
}

int Isx3IsConfPayload::getMagicNumber() {
  return MAGIC_NUMBER_ISX3_IS_CONF_PAYLOAD;
}

} // namespace Devices