#include <configuration_payload.hpp>

using namespace Messages;

namespace Devices {
ConfigurationPayload::ConfigurationPayload(
    DeviceType deviceType, const KeyMapping &keyMapping,
    const SpectrumMapping &spectrumMapping)
    : deviceType(deviceType), keyMapping(keyMapping),
      spectrumMapping(spectrumMapping) {}

ConfigurationPayload::~ConfigurationPayload() {}

DeviceType ConfigurationPayload::getDeviceType() { return this->deviceType; }

KeyMapping ConfigurationPayload::getKeyMapping() { return this->keyMapping; }

SpectrumMapping ConfigurationPayload::getSpectrumMapping() {
  return this->spectrumMapping;
}

void ConfigurationPayload::setSpectrumMapping(
    const SpectrumMapping &spectrumMapping) {
  this->spectrumMapping = spectrumMapping;
}

void ConfigurationPayload::setKeyMapping(const KeyMapping &keyMapping) {
  this->keyMapping = keyMapping;
}

std::string ConfigurationPayload::deviceTypeToString(DeviceType type) {
  if (DeviceType::INVALID == type) {
    return "INVALID";
  } else if (DeviceType::UNSPECIFIED == type) {
    return "UNSPECIFIED";
  } else if (DeviceType::PUMP_CONTROLLER == type) {
    return "Pump Controller";
  } else if (DeviceType::IMPEDANCE_SPECTROMETER == type) {
    return "Impedance Spectrometer";
  } else {
    return "INVALID";
  }
}

} // namespace Devices
