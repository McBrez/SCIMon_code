// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <impedance_spectrometer.hpp>
#include <main_control.hpp>
#include <message_impedance_spectrometer.hpp>

using namespace std;
using namespace Devices;
using namespace Control;

INITIALIZE_EASYLOGGINGPP

int main() {
  LOG(INFO) << "Starting up.";

  LOG(INFO) << "Initializing impedance spectrometer.";
  shared_ptr<Device> impedanceSpectrometer(new ImpedanceSpectrometer());
  impedanceSpectrometer->configure(
      new DeviceConfiguration(IMPEDANCE_SPECTROMETER));

  list<shared_ptr<Device>> inputDevices;
  inputDevices.push_front(impedanceSpectrometer);

  list<shared_ptr<Device>> outputDevices;

  MainControl mainControl(inputDevices, outputDevices);
  mainControl.init();

  while(true) {
    mainControl.run();
  }

  LOG(INFO) << "Shutting down.";
  return 0;
}