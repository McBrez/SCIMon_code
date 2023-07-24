// Project include
#include <common.hpp>
#include <data_manager.hpp>>
#include <is_configuration.hpp>

namespace Devices {

IsConfiguration::IsConfiguration(double frequencyFrom, double frequencyTo,
                                 int measurementPoints, int repetitions)
    : ConfigurationPayload(DeviceType::IMPEDANCE_SPECTROMETER),
      frequencyFrom(frequencyFrom), frequencyTo(frequencyTo),
      measurementPoints(measurementPoints), repetitions(repetitions) {}

IsConfiguration::~IsConfiguration() {}

std::vector<unsigned char> IsConfiguration::bytes() {
  return std::vector<unsigned char>();
}

int IsConfiguration::getMagicNumber() { return MAGIC_NUMBER_IS_CONFIGURATION; }

SpectrumMapping IsConfiguration::constructSpectrumMapping() const {

  SpectrumMapping retVal;

  std::vector<double> frequencies;
  frequencies.reserve(this->measurementPoints);
  double stepSize =
      (this->frequencyTo - this->frequencyFrom) / this->measurementPoints;
  for (int i = 0; i < this->measurementPoints; i++) {
    frequencies.push_back(frequencyFrom + i * stepSize);
  }

  retVal[this->keyMapping.begin()->first] = frequencies;

  return retVal;
}

} // namespace Devices
