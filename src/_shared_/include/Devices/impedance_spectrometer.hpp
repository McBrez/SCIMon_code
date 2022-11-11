#ifndef IMPEDANCE_SPECTROMETER_HPP
#define IMPEDANCE_SPECTROMETER_HPP

#include <device.hpp>

namespace Devices
{

    class ImpedanceSpectrometer : public Device
    {
    public:
        ImpedanceSpectrometer();
        virtual bool configure(DeviceConfiguration *deviceConfiguration) override;
        virtual bool open() override;
        virtual bool close() override;
        virtual bool write(shared_ptr<DeviceMessage> message) override;
        virtual shared_ptr<DeviceMessage> read() override;
    };
}

#endif