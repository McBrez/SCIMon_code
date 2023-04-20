// Project includes
#include <dummy_device.hpp>

namespace Devices {

DummyDevice::DummyDevice() : Device(DeviceType::INVALID) {}

DummyDevice::~DummyDevice() {}

bool DummyDevice::start() { return false; }

bool DummyDevice::stop() { return false; }

bool DummyDevice::initialize(shared_ptr<InitPayload> initPayload) {
  return false;
}

bool DummyDevice::configure(
    shared_ptr<ConfigurationPayload> deviceConfiguration) {
  return false;
}

bool DummyDevice::specificWrite(shared_ptr<WriteDeviceMessage> writeMsg) {
  return false;
}

list<shared_ptr<DeviceMessage>> DummyDevice::specificRead(TimePoint timestamp) {
  return list<shared_ptr<DeviceMessage>>();
}

bool DummyDevice::handleResponse(shared_ptr<ReadDeviceMessage> response) {
  return false;
}

string DummyDevice::getDeviceTypeName() { return "Dummy Device"; }

} // namespace Devices