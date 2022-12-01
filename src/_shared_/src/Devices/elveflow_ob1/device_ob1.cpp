#include <device_ob1.hpp>

namespace Devices {

DeviceOb1::DeviceOb1() {}

DeviceOb1::~DeviceOb1() {}

bool DeviceOb1::configure(shared_ptr<DeviceConfiguration> deviceConfiguration) {
  return true;
}
bool DeviceOb1::start() { return true; }
bool DeviceOb1::stop() { return true; }

bool DeviceOb1::write(shared_ptr<InitDeviceMessage>) { return true; }
bool DeviceOb1::write(shared_ptr<ConfigDeviceMessage>) { return true; }
bool DeviceOb1::write(shared_ptr<WriteDeviceMessage>) { return true; }

string DeviceOb1::getDeviceTypeName() { return ""; }

shared_ptr<ReadDeviceMessage> DeviceOb1::read() {
  return shared_ptr<ReadDeviceMessage>();
}

} // namespace Devices