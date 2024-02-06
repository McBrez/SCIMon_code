// Project includes
#include <dummy_device.hpp>

namespace Devices {

DummyDevice::DummyDevice() : Device(DeviceType::INVALID, 1) {}

DummyDevice::~DummyDevice() {}

bool DummyDevice::start() { return false; }

bool DummyDevice::stop() { return false; }

bool DummyDevice::initialize(std::shared_ptr<InitPayload> initPayload) {
  return false;
}

bool DummyDevice::configure(
    std::shared_ptr<ConfigurationPayload> deviceConfiguration) {
  return false;
}

bool DummyDevice::specificWrite(std::shared_ptr<WriteDeviceMessage> writeMsg) {
  return false;
}

std::list<std::shared_ptr<DeviceMessage>>
DummyDevice::specificRead(TimePoint timestamp) {
  return std::list<std::shared_ptr<DeviceMessage>>();
}

bool DummyDevice::handleResponse(std::shared_ptr<ReadDeviceMessage> response) {
  return false;
}

std::string DummyDevice::getDeviceTypeName() { return "Dummy Device"; }

} // namespace Devices
