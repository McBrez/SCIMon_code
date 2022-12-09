// Project includes
#include <status_payload.hpp>

namespace Devices {

StatusPayload::StatusPayload(DeviceId deviceId, DeviceStatus deviceStatus)
    : deviceId(deviceId), deviceStatus(deviceStatus) {}

DeviceStatus StatusPayload::getDeviceStatus() { return this->deviceStatus; }

DeviceId StatusPayload::getDeviceId() { return this->deviceId; }

string StatusPayload::serialize() { return ""; }

} // namespace Devices
