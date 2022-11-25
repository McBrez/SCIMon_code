#include <device_ob1.hpp>

namespace Devices {

DeviceOb1::DeviceOb1() {}

bool DeviceOb1::configure(DeviceConfiguration *deviceConfiguration) {
  return true;
}
bool DeviceOb1::open() { return true; }
bool DeviceOb1::close() { return true; }

bool DeviceOb1::write(shared_ptr<InitDeviceMessage>) { return true; }
bool DeviceOb1::write(shared_ptr<ConfigDeviceMessage>) { return true; }
bool DeviceOb1::write(shared_ptr<WriteDeviceMessage>) { return true; }

shared_ptr<ReadDeviceMessage> DeviceOb1::read() {
  return shared_ptr<ReadDeviceMessage>();
}

} // namespace Devices