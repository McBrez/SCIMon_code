#ifndef ISX3_IS_CONF_PAYLOAD_HPP
#define ISX3_IS_CONF_PAYLOAD_HPP

// Standard includes
#include <map>
#include <tuple>

// Project includes
#include <is_configuration.hpp>
#include <isx3_constants.hpp>

namespace Devices {
/**
 * @brief Encapsulates a ISX3 Impedance Spectrum measurement configuration.
 */
class Isx3IsConfPayload : public IsConfiguration {
public:
  Isx3IsConfPayload(double frequencyFrom, double frequencyTo,
                    int measurementPoints, int repetitions,
                    std::map<ChannelFunction, int> channel, IsScale scale,
                    MeasurmentConfigurationRange measurementConfRange,
                    MeasurmentConfigurationChannel measurementConfChannel,
                    MeasurementConfiguration measurementConfiguration,
                    const double precision, const double amplitude);

  ~Isx3IsConfPayload();

  /**
   * @brief Serializes the payload into bytes.
   * @return Byte vector, that depicts the payload.
   */
  virtual std::vector<unsigned char> bytes() override;

  /**
   * @brief Returns the magic number of the payload. This number is used to
   * identify the payload type when encoding and decoding payload. This number
   * has to be unique among all payload types.
   */
  int getMagicNumber() override;

  /// maps from channel function to channel number
  std::map<ChannelFunction, int> channel;
  const IsScale scale;
  MeasurmentConfigurationRange measurementConfRange;
  MeasurmentConfigurationChannel measurementConfChannel;
  MeasurementConfiguration measurementConfiguration;
  const double precision;
  const double amplitude;
};
} // namespace Devices

#endif