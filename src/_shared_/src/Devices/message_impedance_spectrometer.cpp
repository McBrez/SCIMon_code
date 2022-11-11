#include <message_impedance_spectrometer.hpp>

namespace Devices
{
    MessageImpedanceSpectrometer::MessageImpedanceSpectrometer(ImpedanceSpectrometerMessageType messageType) : messageType(messageType) {}

    ImpedanceSpectrometerMessageType MessageImpedanceSpectrometer::type()
    {
        return this->messageType;
    }

    string MessageImpedanceSpectrometer::serialize()
    {
        return "";
    }
}