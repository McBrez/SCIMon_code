// Standard includes
#include <iostream>
#include <chrono>
#include <ctime>

// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <impedance_spectrometer.hpp>
#include <message_impedance_spectrometer.hpp>

using namespace Devices;
using namespace std;

INITIALIZE_EASYLOGGINGPP

int main()
{
    LOG(INFO) << "Starting up.";

    LOG(INFO) << "Initializing impedance spectrometer.";
    Device *impedanceSpectrometer = new ImpedanceSpectrometer();
    impedanceSpectrometer->configure(new DeviceConfiguration(IMPEDANCE_SPECTROMETER));

    shared_ptr<DeviceMessage> initMessage(new MessageImpedanceSpectrometer(ImpedanceSpectrometerMessageType::INITIALIZE));
    impedanceSpectrometer->write(initMessage);

    LOG(INFO) << "Shutting down.";
    return 0;
}