#include <is_configuration.hpp>

namespace Devices {

IsConfiguration::IsConfiguration(double frequencyFrom, double frequencyTo,
                                 int measurementPoints, int repetitions,
                                 string channel, IsScale scale,
                                 double precision, double amplitude,
                                 int impdedanceRange, int frequencyRange)
    : DeviceConfiguration(DeviceType::IMPEDANCE_SPECTROMETER),
      frequencyFrom(frequencyFrom), frequencyTo(frequencyTo),
      measurementPoints(measurementPoints), repetitions(repetitions),
      channel(channel), scale(scale), precision(precision),
      amplitude(amplitude), impedanceRange(impedanceRange),
      frequencyRange(frequencyRange) {}

} // namespace Devices
