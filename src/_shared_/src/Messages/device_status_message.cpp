#include <device_status_message.hpp>

namespace Messages {

DeviceStatusMessage::DeviceStatusMessage(DeviceStatus deviceStatus,
                                         string additionalInformation)
    : ReadDeviceMessage(additionalInformation), deviceStatus(deviceStatus) {}

DeviceStatus DeviceStatusMessage::getDeviceStatus() {
  return this->deviceStatus;
}
} // namespace Messages