// Project includes
#include <common.hpp>
#include <isx3_is_conf_payload.hpp>

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
  return std::vector<unsigned char>();
}

int Isx3IsConfPayload::getMagicNumber() {
  return MAGIC_NUMBER_ISX3_IS_CONF_PAYLOAD;
}

} // namespace Devices