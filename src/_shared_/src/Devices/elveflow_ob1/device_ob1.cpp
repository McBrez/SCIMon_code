// Project includes
#include <common.hpp>
#include <device_ob1.hpp>

namespace Devices {

DeviceOb1::DeviceOb1() : Device(DeviceType::PUMP_CONTROLLER, 1) {}

DeviceOb1::~DeviceOb1() {}

std::string DeviceOb1::getDeviceTypeName() { return OB1_DEVICE_TYPE_NAME; }

bool DeviceOb1::handleResponse(std::shared_ptr<ReadDeviceMessage> response) {
  // Device does not expect responses. Just return true here.
  return true;
}

} // namespace Devices
