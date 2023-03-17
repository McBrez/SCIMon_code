// Project includes
#include <isx3_is_conf_payload.hpp>

namespace Devices {

Isx3IsConfPayload::Isx3IsConfPayload(
    double frequencyFrom, double frequencyTo, int measurementPoints,
    int repetitions, map<ChannelFunction, int> channel, IsScale scale,
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

} // namespace Devices