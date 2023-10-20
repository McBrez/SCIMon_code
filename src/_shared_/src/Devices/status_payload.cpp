// Project includes
#include <common.hpp>
#include <status_payload.hpp>

// 3rd party includes
#include <flatbuffers/flatbuffers.h>

// Generated includes
#include <status_payload_generated.h>

namespace Devices {

StatusPayload::StatusPayload(
    UserId deviceId, DeviceStatus deviceStatus, std::list<UserId> proxyIds,
    DeviceType deviceType, std::string deviceName,
    std::shared_ptr<InitPayload> initPayload,
    std::shared_ptr<ConfigurationPayload> configPayload)
    : deviceStatus(deviceStatus), deviceId(deviceId), proxyIds(proxyIds),
      deviceType(deviceType), deviceName(deviceName), initPayload(initPayload),
      configPayload(configPayload) {}

StatusPayload::StatusPayload(UserId deviceId, DeviceStatus deviceStatus,
                             std::list<UserId> proxyIds, DeviceType deviceType,
                             std::string deviceName, InitPayload *initPayload,
                             ConfigurationPayload *configPayload)
    : deviceStatus(deviceStatus), deviceId(deviceId), proxyIds(proxyIds),
      deviceType(deviceType), deviceName(deviceName), initPayload(initPayload),
      configPayload(configPayload) {}

DeviceStatus StatusPayload::getDeviceStatus() { return this->deviceStatus; }

UserId StatusPayload::getDeviceId() { return this->deviceId; }

DeviceType StatusPayload::getDeviceType() { return this->deviceType; }

std::string StatusPayload::getDeviceName() { return this->deviceName; }

std::string StatusPayload::serialize() { return ""; }

std::list<UserId> StatusPayload::getProxyIds() { return this->proxyIds; }

bool StatusPayload::operator==(const StatusPayload &other) const {
  return this->deviceId == other.deviceId &&
         this->deviceName == other.deviceName &&
         this->deviceStatus == other.deviceStatus &&
         this->deviceType == other.deviceType &&
         this->proxyIds == other.proxyIds;
}

std::vector<unsigned char> StatusPayload::bytes() {
  Serialization::Devices::StatusPayloadT intermediateObject;

  intermediateObject.deviceId = this->deviceId.id();
  intermediateObject.deviceName = this->deviceName;
  intermediateObject.deviceStatus =
      static_cast<Serialization::Devices::DeviceStatus>(this->deviceStatus);
  intermediateObject.deviceType =
      static_cast<Serialization::Devices::DeviceType>(this->deviceType);
  std::vector<size_t> proxyIds;
  for (auto proxyId : this->proxyIds) {
    proxyIds.push_back(proxyId.id());
  }
  intermediateObject.proxyIds = proxyIds;
  if (this->initPayload) {
    intermediateObject.initPayloadMagicNumber =
        this->initPayload->getMagicNumber();
    intermediateObject.initPayload = this->initPayload->bytes();
  } else {
    intermediateObject.initPayloadMagicNumber = -1;
  }
  if (this->configPayload) {
    intermediateObject.configPayloadMagicNumber =
        this->configPayload->getMagicNumber();
    intermediateObject.configPayload = this->configPayload->bytes();
  } else {
    intermediateObject.configPayloadMagicNumber = -1;
  }

  // Serialize the intermediate object.
  flatbuffers::FlatBufferBuilder builder;
  builder.Finish(Serialization::Devices::StatusPayload::Pack(
      builder, &intermediateObject));
  uint8_t *buffer = builder.GetBufferPointer();
  std::vector<unsigned char> bufferVect = std::vector<unsigned char>(
      (const unsigned char *)buffer,
      (const unsigned char *)buffer + builder.GetSize());

  return bufferVect;
}

int StatusPayload::getMagicNumber() { return MAGIC_NUMBER_STATUS_PAYLOAD; }

std::shared_ptr<ConfigurationPayload> StatusPayload::getConfigurationPayload() {
  return this->configPayload;
}

std::shared_ptr<InitPayload> StatusPayload::getInitPayload() {
  return this->initPayload;
}
} // namespace Devices
