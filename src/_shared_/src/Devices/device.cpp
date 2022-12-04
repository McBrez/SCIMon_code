// Project includes
#include <device.hpp>

namespace Devices {
Device::Device()
    : configurationFinished(false), initFinished(false),
      deviceState(DeviceStatus::UNKNOWN_DEVICE_STATUS) {}

Device::~Device() {}

DeviceId Device::getDeviceId() { return this->deviceId; }

DeviceStatus Device::getDeviceStatus() { return this->deviceState; }

bool Device::isConfigured() { return this->configurationFinished; }

bool Device::isInitialized() { return this->initFinished; }

list<shared_ptr<DeviceMessage>> Device::readN(unsigned int n) {
  return list<shared_ptr<DeviceMessage>>();
}
} // namespace Devices