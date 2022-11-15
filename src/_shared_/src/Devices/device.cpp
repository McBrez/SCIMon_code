#include <device.hpp>

namespace Devices {
Device::Device() : configurationFinished(false) {}

bool Device::isConfigured() { return this->configurationFinished; }

bool Device::isInitialized() { return this->initFinished; }

list<shared_ptr<DeviceMessage>> Device::readN(unsigned int n) {
  return list<shared_ptr<DeviceMessage>>();
}
} // namespace Devices