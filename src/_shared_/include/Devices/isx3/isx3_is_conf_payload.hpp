#ifndef ISX3_IS_CONF_PAYLOAD_HPP
#define ISX3_IS_CONF_PAYLOAD_HPP

// Standard includes
#include <map>
#include <tuple>

// Project includes
#include <is_configuration.hpp>
#include <isx3_constants.hpp>

using namespace std;

namespace Devices {
/**
 * @brief Encapsulates a ISX3 Impedance Spectrum measurement configuration.
 */
class Isx3IsConfPayload : public IsConfiguration {
public:
  Isx3IsConfPayload(double frequencyFrom, double frequencyTo,
                    int measurementPoints, int repetitions,
                    map<ChannelFunction, int> channel, IsScale scale,
                    MeasurmentConfigurationRange measurementConfRange,
                    MeasurmentConfigurationChannel measurementConfChannel,
                    MeasurementConfiguration measurementConfiguration,
                    const double precision, const double amplitude);

  ~Isx3IsConfPayload();

  /// maps from channel function to channel number
  const map<ChannelFunction, int> channel;
  const IsScale scale;
  MeasurmentConfigurationRange measurementConfRange;
  MeasurmentConfigurationChannel measurementConfChannel;
  MeasurementConfiguration measurementConfiguration;
  const double precision;
  const double amplitude;
};
} // namespace Devices

#endif