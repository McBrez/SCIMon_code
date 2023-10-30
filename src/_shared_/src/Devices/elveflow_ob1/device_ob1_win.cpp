// 3rd party includes
#include <Elveflow64_shim.h>

// Project includes
#include <device_ob1_win.hpp>
#include <easylogging++.h>
#include <ob1_constants.hpp>
#include <ob1_init_payload.hpp>
#include <read_payload_ob1.hpp>
#include <set_pressure_payload.hpp>

namespace Devices {

DeviceOb1Win::DeviceOb1Win()
    : DeviceOb1(), ob1Id(-1),
      calibration(new double[Constants::Ob1CalibrationArrayLen]),
      cachedPressures({{1, 0.0}, {2, 0.0}, {3, 0.0}, {4, 0.0}}) {}

DeviceOb1Win::~DeviceOb1Win() {
  OB1_Destructor(ob1Id);
  delete[] calibration;
  if (this->configurationThread) {
    this->configurationThread->join();
  }
}

bool DeviceOb1Win::initialize(std::shared_ptr<InitPayload> initPayload) {

  // Get the payload of the message and check if it is the correct type.
  std::shared_ptr<Ob1InitPayload> ob1InitPayload =
      dynamic_pointer_cast<Ob1InitPayload>(initPayload);
  if (!initPayload) {
    LOG(ERROR) << "Received an ill-formed init message.";
    return false;
  }

  this->deviceState = DeviceStatus::INITIALIZING;

  char *deviceName = new char[ob1InitPayload->getDeviceName().length() + 1];
  strcpy(deviceName, ob1InitPayload->getDeviceName().c_str());
  int error = OB1_Initialization(
      deviceName, get<0>(ob1InitPayload->getChannelConfig()),
      get<1>(ob1InitPayload->getChannelConfig()),
      get<2>(ob1InitPayload->getChannelConfig()),
      get<3>(ob1InitPayload->getChannelConfig()), &this->ob1Id);
  delete[] deviceName;

  // Was init successful?
  if (this->ob1Id != -1) {
    // It was.
    this->initFinished = true;
    this->deviceState = DeviceStatus::INITIALIZED;
    this->ob1DeviceName = ob1InitPayload->getDeviceName();
    this->onInitialized(this->getDeviceSerialNumber(), Utilities::KeyMapping(),
                        Utilities::SpectrumMapping());

    return true;
  } else {
    // It was not.
    this->initFinished = false;
    this->deviceState = DeviceStatus::UNKNOWN_DEVICE_STATUS;

    return false;
  }
}

void DeviceOb1Win::configureWorker(
    std::shared_ptr<ConfigurationPayload> deviceConfiguration) {
  this->deviceState = DeviceStatus::CONFIGURING;
  this->configurationFinished = false;
  LOG(INFO) << "Starting calibration of OB1.";
  int retVal = Elveflow_Calibration_Default(this->calibration,
                                            Constants::Ob1CalibrationArrayLen);
  if (retVal == 0) {
    LOG(INFO) << "Finished calibration of OB1 successfully.";
    this->configurationFinished = true;
    this->deviceState = DeviceStatus::IDLE;
    this->onConfigured(this->configPayload->getKeyMapping(),
                       this->configPayload->getSpectrumMapping());

    return;
  } else {
    LOG(INFO) << "Finished calibration of OB1 with an error.";
    this->configurationFinished = false;
    this->deviceState = DeviceStatus::UNKNOWN_DEVICE_STATUS;
    return;
  }

  return;
}

bool DeviceOb1Win::start() {
  if (this->initFinished == true && this->configurationFinished == true) {
    if (this->deviceState == DeviceStatus::IDLE) {
      LOG(DEBUG) << "Starting OB1...";
      // Restore cached pressures.
      for (auto it : this->cachedPressures) {
        int retVal =
            OB1_Set_Press(this->ob1Id, it.first, it.second, this->calibration,
                          Constants::Ob1CalibrationArrayLen);
        LOG(DEBUG) << "Set pressure " << it.second << " on channel " << it.first
                   << ", with return value " << retVal;
      }
      this->deviceState = DeviceStatus::OPERATING;
      return true;
    } else {
      LOG(WARNING) << "OB1 can not be started, as it is already running, or in "
                      "an invalid state.";
      return false;
    }
  } else {
    LOG(WARNING)
        << "OB1 cannot be started, as it is not yet initialized or configured.";
    return false;
  }
}

bool DeviceOb1Win::stop() {
  if (this->initFinished == true && this->configurationFinished == true) {
    if (this->deviceState == DeviceStatus::OPERATING) {
      LOG(DEBUG) << "Stopping OB1...";
      // Set pressures to zero.
      double pressures[] = {0.0, 0.0, 0.0, 0.0};
      OB1_Set_All_Press(this->ob1Id, pressures, this->calibration, 4,
                        Constants::Ob1CalibrationArrayLen);
      this->deviceState = DeviceStatus::IDLE;
      return true;
    } else {
      LOG(WARNING) << "OB1 can not be stopped, as it is already stopped, or in "
                      "an invalid state.";
      return false;
    }
  } else {
    LOG(WARNING)
        << "OB1 cannot be stopped, as it is not yet initialized or configured.";
    return false;
  }
}

bool DeviceOb1Win::configure(
    std::shared_ptr<ConfigurationPayload> configPayload) {
  // Create a new thread and start it.
  this->configurationThread.reset(
      new std::thread(&DeviceOb1Win::configureWorker, this, configPayload));
  return true;
}

std::list<std::shared_ptr<DeviceMessage>>
DeviceOb1Win::specificRead(TimePoint timestamp) {
  // Only read, when in the correct state.
  if (this->deviceState != DeviceStatus::OPERATING) {
    return std::list<std::shared_ptr<DeviceMessage>>();
  }

  double pressureCh1;
  double pressureCh2;
  double pressureCh3;
  double pressureCh4;
  OB1_Get_Press(this->ob1Id, 0, 1, this->calibration, &pressureCh1,
                Constants::Ob1CalibrationArrayLen);
  OB1_Get_Press(this->ob1Id, 1, 0, this->calibration, &pressureCh2,
                Constants::Ob1CalibrationArrayLen);
  OB1_Get_Press(this->ob1Id, 2, 0, this->calibration, &pressureCh3,
                Constants::Ob1CalibrationArrayLen);
  OB1_Get_Press(this->ob1Id, 3, 0, this->calibration, &pressureCh4,
                Constants::Ob1CalibrationArrayLen);

  ReadPayloadOb1 *readPayload = new ReadPayloadOb1(
      std::make_tuple(pressureCh1, pressureCh2, pressureCh3, pressureCh4));

  LOG(DEBUG) << readPayload->serialize();
  std::list<std::shared_ptr<DeviceMessage>> retVal;
  retVal.emplace_back(new ReadDeviceMessage(
      this->self->getUserId(), this->startMessageCache->getSource(),
      ReadDeviceTopic::READ_TOPIC_DEVICE_SPECIFIC_MSG, readPayload,
      this->startMessageCache));
  return retVal;
}

bool DeviceOb1Win::specificWrite(std::shared_ptr<WriteDeviceMessage> writeMsg) {
  LOG(DEBUG) << "OB1 received a device specific message.";

  // Try to cast down the payload.
  // Is it a set pressure payload?
  auto setPressurePayload =
      dynamic_pointer_cast<SetPressurePayload>(writeMsg->getPayload());
  if (setPressurePayload) {
    // It is a set pressure message. Set them now.
    std::vector<double> setPressures = setPressurePayload->getPressures();
    for (int i = 0; i > setPressures.size(); i++) {
      int retVal =
          OB1_Set_Press(this->ob1Id, i + 1, setPressures[i], this->calibration,
                        Constants::Ob1CalibrationArrayLen);
      this->cachedPressures[i + 1] = setPressures[i];
      LOG(DEBUG) << "Set pressure " << setPressures[i] << " on channel "
                 << i + 1 << ", with return value " << retVal;
    }

    return true;
  } else {
    // Unsupported write message received.
    return false;
  }
}
std::string DeviceOb1Win::getDeviceSerialNumber() {
  return this->ob1DeviceName;
}

} // namespace Devices
