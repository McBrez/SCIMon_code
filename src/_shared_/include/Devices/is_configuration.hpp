#ifndef IS_CONFIGURATION_HPP
#define IS_CONFIGURATION_HPP

// Standard includes
#include <string>

// Project includes
#include <configuration_payload.hpp>

namespace Devices {

enum IsScale { LINEAR_SCALE = 0x00, LOGARITHMIC_SCALE = 0x01 };

/**
 * Encapsulates the configuration on an impedance spectrum measurement.
 */
class IsConfiguration : public ConfigurationPayload {
public:
  /**
   * @brief IsConfiguration
   * @param frequencyFrom
   * @param frequencyTo
   * @param measurementPoints
   * @param repetitions
   * @param keyMapping
   */
  IsConfiguration(double frequencyFrom, double frequencyTo,
                  int measurementPoints, int repetitions);

  virtual ~IsConfiguration();

  /// The starting frequency of the spectrum.
  const double frequencyFrom;
  /// The stopping frequency of the spectrum.
  const double frequencyTo;
  /// The count of points betwenn starting and stopping frequency.
  const int measurementPoints;
  /// Count of impedance spectrums, that shall be gathered. Value of <= 0
  /// indicates unlimited measurement.
  const int repetitions;

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
  virtual int getMagicNumber() override;

  /**
   * @brief Constructs a spectrumMapping from the held data.
   * @return A spectrum mapping that represents the held configuration data.
   */
  SpectrumMapping constructSpectrumMapping() const;
};
} // namespace Devices

#endif
