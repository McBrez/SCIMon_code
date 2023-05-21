// Project includes
#include <status_payload.hpp>

namespace Devices {

StatusPayload::StatusPayload(UserId deviceId, DeviceStatus deviceStatus,
                             list<UserId> proxyIds, DeviceType deviceType,
                             string deviceName)
    : deviceId(deviceId), deviceStatus(deviceStatus) {}

DeviceStatus StatusPayload::getDeviceStatus() { return this->deviceStatus; }

UserId StatusPayload::getDeviceId() { return this->deviceId; }

DeviceType StatusPayload::getDeviceType() { return this->deviceType; }

string StatusPayload::getDeviceName() { return this->deviceName; }

string StatusPayload::serialize() { return ""; }

list<UserId> StatusPayload::getProxyIds() { return this->proxyIds; }

bool StatusPayload::operator==(const StatusPayload &other) const {
  return this->deviceId == other.deviceId &&
         this->deviceName == other.deviceName &&
         this->deviceStatus == other.deviceStatus &&
         this->deviceType == other.deviceType &&
         this->proxyIds == other.proxyIds;
}

vector<unsigned char> StatusPayload::bytes() { return vector<unsigned char>(); }

} // namespace Devices
