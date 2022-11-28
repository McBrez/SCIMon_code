#ifndef READ_SPECTRUM_MESSAGE_HPP
#define READ_SPECTRUM_MESSAGE_HPP

// Standard includes
#include <complex>

// Project includes
#include <read_device_message.hpp>

// using Impedance = std::complex<double>;

namespace Messages {
class ReadSpectrumMessage : public ReadDeviceMessage {
public:
  ReadSpectrumMessage(double frequency, std::complex<double> impedance,
                      string data);

private:
  double frequency;
  std::complex<double> impedance;
};
} // namespace Messages

#endif