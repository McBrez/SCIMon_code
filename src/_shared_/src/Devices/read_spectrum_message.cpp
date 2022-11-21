// Project includes
#include <read_spectrum_message.hpp>

namespace Devices {
ReadSpectrumMessage::ReadSpectrumMessage(double frequency,
                                         std::complex<double> impedance,
                                         string data)
    : ReadDeviceMessage(data), frequency(frequency), impedance(impedance) {}
} // namespace Devices