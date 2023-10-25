// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <data_response_payload.hpp>
#include <device.hpp>
#include <dummy_message.hpp>
#include <key_response_payload.hpp>>
#include <request_data_payload.hpp>
#include <status_payload.hpp>

using namespace Utilities;

namespace Devices {

Device::Device(DeviceType deviceType, DataManagerType dataManagerType)
    : MessageInterface(deviceType, dataManagerType),
      configurationFinished(false), initFinished(false),
      startMessageCache(new Messages::DummyMessage()) {
  this->deviceType = deviceType;
}

Device::~Device() {}

std::string Device::deviceStatusToString(DeviceStatus deviceStatus) {

  if (UNKNOWN_DEVICE_STATUS == deviceStatus)
    return "UNKNOWN DEVICE STATUS";
  else if (INITIALIZING == deviceStatus)
    return "INITIALIZING";
  else if (INITIALIZED == deviceStatus)
    return "INITIALIZED";
  else if (CONFIGURING == deviceStatus)
    return "CONFIGURING";
  else if (OPERATING == deviceStatus)
    return "OPERATING";
  else if (IDLE == deviceStatus)
    return "IDLE";
  else if (BUSY == deviceStatus)
    return "BUSY";
  else if (ERROR == deviceStatus)
    return "ERROR";
  else
    return "";
}

std::shared_ptr<StatusPayload> Device::constructStatus() {
  return std::shared_ptr<StatusPayload>(new StatusPayload(
      this->getUserId(), this->getDeviceStatus(), this->getProxyUserIds(),
      this->getDeviceType(), this->getDeviceTypeName(), this->initPayload,
      this->configPayload));
}

} // namespace Devices
