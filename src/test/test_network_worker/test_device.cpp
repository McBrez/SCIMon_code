// Project includes
#include <test_device.hpp>

using namespace Devices;

TestDevice::TestDevice() : Device(DeviceType::UNSPECIFIED) {}

TestDevice::~TestDevice() {}

string TestDevice::getDeviceTypeName() { return "TEST DEVICE"; }

bool TestDevice::specificWrite(shared_ptr<WriteDeviceMessage> writeMsg) {
  this->messageOut.push(new ReadDeviceMessage(
      this->getUserId(), writeMsg->getSource(),
      ReadDeviceTopic::READ_TOPIC_DEVICE_SPECIFIC_MSG, , writeMsg));

  return false;
}

list<shared_ptr<DeviceMessage>> TestDevice::specificRead(TimePoint timestamp) {
  return list<shared_ptr<DeviceMessage>>();
}

bool TestDevice::handleResponse(shared_ptr<ReadDeviceMessage> response) {
  return false;
}

bool TestDevice::configure(
    shared_ptr<ConfigurationPayload> deviceConfiguration) {
  return true;
}

bool TestDevice::initialize(shared_ptr<InitPayload> initPayload) {
  return true;
}

bool TestDevice::start() { return true; }

bool TestDevice::stop() { return true; }
