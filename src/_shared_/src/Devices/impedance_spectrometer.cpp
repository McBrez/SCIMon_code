// 34d party includes
#include <easylogging++.h>

// Project includes
#include <impedance_spectrometer.hpp>
#include <message_impedance_spectrometer.hpp>

namespace Devices
{

    ImpedanceSpectrometer::ImpedanceSpectrometer() : Device() {}

    bool ImpedanceSpectrometer::configure(DeviceConfiguration *deviceConfiguration) {}

    bool ImpedanceSpectrometer::open() {}

    bool ImpedanceSpectrometer::close() {}

    bool ImpedanceSpectrometer::write(shared_ptr<DeviceMessage> message)
    {
        shared_ptr<MessageImpedanceSpectrometer> castedMessage = dynamic_pointer_cast<MessageImpedanceSpectrometer>(message);

        if (castedMessage->type() == ImpedanceSpectrometerMessageType::CALIBRATE)
        {
        }
        else if (castedMessage->type() == ImpedanceSpectrometerMessageType::GET_DATA)
        {
        }
        else
        {
            LOG(ERROR) << "Received an invalid message. "
                       << "The message will be ignored. This will most likely result in "
                       << "unpredictable behaviour.";
            return false;
        }
    }

    shared_ptr<DeviceMessage> ImpedanceSpectrometer::read() {}
}
