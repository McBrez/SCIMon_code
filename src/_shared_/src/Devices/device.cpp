#include <device.hpp>

namespace Devices
{
    Device::Device() : configurationFinished(false) {}

    bool Device::isConfigured() { return this->configurationFinished; }

    list<shared_ptr<DeviceMessage>> Device::readN(unsigned int n)
    {
        return list<shared_ptr<DeviceMessage>>();
    }
}