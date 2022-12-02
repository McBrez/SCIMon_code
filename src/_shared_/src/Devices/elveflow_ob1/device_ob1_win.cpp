// 3rd party includes
#include <Elveflow64.h>

// Project includes
#include <device_ob1_win.hpp>
#include <easylogging++.h>

namespace Devices {

DeviceOb1Win::DeviceOb1Win()
    : DeviceOb1(), MyOB1_ID(-1), calibration(new double[1000]) {}

DeviceOb1Win::~DeviceOb1Win() {
  OB1_Destructor(MyOB1_ID);
  delete calibration;
}

bool DeviceOb1Win::write(shared_ptr<InitDeviceMessage> initMsg) {
  int error = 0;

  int MyOB1_ID = -1; // initialize myOB1ID at negative value (after
  // initialization it should become positive or =0)
  // initialize the OB1 -> Use NIMAX to determine the device
  // name avoid non alphanumeric characters in device name
  char *deviceName =
      new char[initMsg->returnPayload()->deviceName.length() + 1];
  strcpy(deviceName, initMsg->returnPayload()->deviceName.c_str());

  error = OB1_Initialization(
      deviceName, initMsg->returnPayload()->channelOneConnfig,
      initMsg->returnPayload()->channelTwoConnfig,
      initMsg->returnPayload()->channelThreeConnfig,
      initMsg->returnPayload()->channelFourConnfig, &MyOB1_ID);
  delete[] deviceName;

  return MyOB1_ID != -1;
}

bool DeviceOb1Win::configure(
    shared_ptr<DeviceConfiguration> deviceConfiguration) {

  int retVal = Elveflow_Calibration_Default(this->calibration, 1000);

  return retVal == 0;
}

bool DeviceOb1Win::start() { return false; }
bool DeviceOb1Win::stop() { return false; }
bool DeviceOb1Win::write(shared_ptr<ConfigDeviceMessage>) { return false; }
bool DeviceOb1Win::write(shared_ptr<WriteDeviceMessage>) { return false; }
shared_ptr<ReadDeviceMessage> DeviceOb1Win::read() {
  double pressureCh1;
  double pressureCh2;
  double pressureCh3;
  double pressureCh4;
  OB1_Get_Press(this->MyOB1_ID, 0, 1, this->calibration, &pressureCh1, 1000);
  OB1_Get_Press(this->MyOB1_ID, 1, 1, this->calibration, &pressureCh2, 1000);
  OB1_Get_Press(this->MyOB1_ID, 2, 1, this->calibration, &pressureCh3, 1000);
  OB1_Get_Press(this->MyOB1_ID, 3, 1, this->calibration, &pressureCh4, 1000);

  string data;
  data += "Ch1: " + to_string(pressureCh1) + "mBar\n";
  data += "Ch2: " + to_string(pressureCh2) + "mBar\n";
  data += "Ch3: " + to_string(pressureCh3) + "mBar\n";
  data += "Ch4: " + to_string(pressureCh4) + "mBar\n";

  LOG(DEBUG) << data;
  return shared_ptr<ReadDeviceMessage>(new ReadDeviceMessage(data));
}

} // namespace Devices