#include <is_configuration.hpp>

namespace Devices {

IsConfiguration::IsConfiguration(double frequencyFrom, double frequencyTo,
                                 int measurementPoints, int repetitions)
    : ConfigurationPayload(DeviceType::IMPEDANCE_SPECTROMETER),
      frequencyFrom(frequencyFrom), frequencyTo(frequencyTo),
      measurementPoints(measurementPoints), repetitions(repetitions) {}

IsConfiguration::~IsConfiguration() {}

} // namespace Devices
