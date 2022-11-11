#ifndef DEVICE_HPP
#define DEVICE_HPP

// Standard includes
#include <memory>
#include <list>

// Project includes
#include <device_configuration.hpp>
#include <device_message.hpp>

using namespace std;

namespace Devices
{
    /**
     * @brief Depicts a physical device, like a measurement aparatus or a
     * pump controller. A device can be configured, can be read to and written
     * from.
     */
    class Device
    {
    protected:
        bool configurationFinished;
        shared_ptr<DeviceConfiguration> deviceConfiguration;

    public:
        Device();

        virtual bool configure(DeviceConfiguration *deviceConfiguration) = 0;
        virtual bool open() = 0;
        virtual bool close() = 0;

        bool isConfigured();
        virtual bool write(shared_ptr<DeviceMessage>) = 0;
        virtual shared_ptr<DeviceMessage> read() = 0;
        list<shared_ptr<DeviceMessage>> readN(unsigned int n);
    };
}

#endif
