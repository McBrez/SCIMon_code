#ifndef IS_CONFIGURATION_HPP
#define IS_CONFIGURATION_HPP

// Standard includes
#include <string>

// Project includes
#include <device_configuration.hpp>

using namespace std;

namespace Devices {

enum IsScale { LINEAR_SCALE, LOGARITHMIC_SCALE };

/**
 * Encapsulates the configuration on an impedance spectrum measurement.
 */
class IsConfiguration : public DeviceConfiguration {
public:
  IsConfiguration(double frequencyFrom, double frequencyTo,
                  int measurementPoints, int repetitions, string channel,
                  IsScale scale, double precision, double amplitude,
                  int impdedanceRange, int frequencyRange);

  const double frequencyFrom;
  const double frequencyTo;
  const int measurementPoints;
  const int repetitions;
  const string channel;
  const IsScale scale;
  const double precision;
  const double amplitude;
  const int impedanceRange;
  const int frequencyRange;
};
} // namespace Devices

#endif