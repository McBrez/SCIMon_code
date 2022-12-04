// 3rd party includes
#include <Elveflow64.h>

// Project includes
#include <device_ob1_win.hpp>
#include <easylogging++.h>
#include <init_payload_ob1.hpp>
#include <ob1_constants.hpp>
#include <read_payload_ob1.hpp>

namespace Devices {

DeviceOb1Win::DeviceOb1Win()
    : DeviceOb1(), ob1Id(-1),
      calibration(new double[Constants::Ob1CalibrationArrayLen]) {}

DeviceOb1Win::~DeviceOb1Win() {
  OB1_Destructor(ob1Id);
  delete[] calibration;
}

bool DeviceOb1Win::write(shared_ptr<InitDeviceMessage> initMsg) {
  // Is the payload meant for this device?
  if (this->deviceId != initMsg->getTargetDeviceId()) {
    LOG(WARNING)
        << "Received an init message that is not meant for this device.";
    return false;
  }

  // Get the payload of the message and check if it is the correct type.
  shared_ptr<InitPayloadOb1> initPayload =
      dynamic_pointer_cast<InitPayloadOb1>(initMsg->returnPayload());
  if (!initPayload) {
    LOG(ERROR) << "Received an ill-formed init message.";
    return false;
  }

  char *deviceName = new char[initPayload->getDeviceName().length() + 1];
  strcpy(deviceName, initPayload->getDeviceName().c_str());

  int error =
      OB1_Initialization(deviceName, get<0>(initPayload->getChannelConfig()),
                         get<1>(initPayload->getChannelConfig()),
                         get<2>(initPayload->getChannelConfig()),
                         get<3>(initPayload->getChannelConfig()), &this->ob1Id);
  delete[] deviceName;

  // Was init successful?
  if (this->ob1Id != -1) {
    // It was.
    this->initFinished = true;
    return true;
  } else {
    // It was not.
    this->initFinished = false;
    return true;
  }
}

bool DeviceOb1Win::configure(
    shared_ptr<DeviceConfiguration> deviceConfiguration) {

  int retVal = Elveflow_Calibration_Default(this->calibration,
                                            Constants::Ob1CalibrationArrayLen);

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
  OB1_Get_Press(this->ob1Id, 0, 1, this->calibration, &pressureCh1, 1000);
  OB1_Get_Press(this->ob1Id, 1, 0, this->calibration, &pressureCh2, 1000);
  OB1_Get_Press(this->ob1Id, 2, 0, this->calibration, &pressureCh3, 1000);
  OB1_Get_Press(this->ob1Id, 3, 0, this->calibration, &pressureCh4, 1000);

  ReadPayloadOb1 *readPayload = new ReadPayloadOb1(
      make_tuple(pressureCh1, pressureCh2, pressureCh3, pressureCh4));

  LOG(DEBUG) << readPayload->serialize();
  return shared_ptr<ReadDeviceMessage>(new ReadDeviceMessage(
      ReadDeviceTopic::READ_TOPIC_OB1_DEVICE_IMAGE, readPayload));
}

} // namespace Devices