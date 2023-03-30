#ifndef IS_CONFIGURATION_HPP
#define IS_CONFIGURATION_HPP

// Standard includes
#include <string>

// Project includes
#include <configuration_payload.hpp>

using namespace std;

namespace Devices {

enum IsScale { LINEAR_SCALE, LOGARITHMIC_SCALE };

/**
 * Encapsulates the configuration on an impedance spectrum measurement.
 */
class IsConfiguration : public ConfigurationPayload {
public:
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
};
} // namespace Devices

#endif