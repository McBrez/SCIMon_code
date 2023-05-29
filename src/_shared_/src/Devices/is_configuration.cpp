// Project include
#include <common.hpp>
#include <is_configuration.hpp>


namespace Devices {

IsConfiguration::IsConfiguration(double frequencyFrom, double frequencyTo,
                                 int measurementPoints, int repetitions)
    : ConfigurationPayload(DeviceType::IMPEDANCE_SPECTROMETER),
      frequencyFrom(frequencyFrom), frequencyTo(frequencyTo),
      measurementPoints(measurementPoints), repetitions(repetitions) {}

IsConfiguration::~IsConfiguration() {}

vector<unsigned char> IsConfiguration::bytes() {
  return vector<unsigned char>();
}

int IsConfiguration::getMagicNumber() { return MAGIC_NUMBER_IS_CONFIGURATION; }

} // namespace Devices
