#include <device_ob1.hpp>

namespace Devices {

DeviceOb1::DeviceOb1() : Device(DeviceType::PUMP_CONTROLLER) {}

DeviceOb1::~DeviceOb1() {}

string DeviceOb1::getDeviceTypeName() { return ""; }

bool DeviceOb1::handleResponse(shared_ptr<ReadDeviceMessage> response) {
  // Device does not expect responses. Just return true here.
  return true;
}

} // namespace Devices