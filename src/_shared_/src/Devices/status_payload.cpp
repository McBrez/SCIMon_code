// Project includes
#include <status_payload.hpp>

namespace Devices {

StatusPayload::StatusPayload(UserId deviceId, DeviceStatus deviceStatus)
    : deviceId(deviceId), deviceStatus(deviceStatus) {}

DeviceStatus StatusPayload::getDeviceStatus() { return this->deviceStatus; }

UserId StatusPayload::getDeviceId() { return this->deviceId; }

string StatusPayload::serialize() { return ""; }

} // namespace Devices
