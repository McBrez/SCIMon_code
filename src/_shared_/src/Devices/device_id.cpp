// Standard includes
#include <climits>

// Project includes
#include <device_id.hpp>

namespace Devices {

DeviceId::DeviceId() { this->deviceId = this->generateDeviceId(); }

DeviceId::DeviceId(int deviceId) : deviceId(deviceId) {}

int DeviceId::id() const { return this->deviceId; }

int DeviceId::generateDeviceId() {
  static int currentId = 0;

  if (currentId == INT_MAX) {
    return -1;
  } else {
    return currentId++;
  }
}

bool DeviceId::operator==(const DeviceId &other) {
  return this->deviceId == other.id();
}

bool DeviceId::operator!=(const DeviceId &other) {
  return this->deviceId != other.id();
}

} // namespace Devices