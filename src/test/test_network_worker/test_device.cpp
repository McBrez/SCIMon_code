// Project includes
#include <common.hpp>
#include <generic_read_payload.hpp>
#include <message_distributor.hpp>
#include <test_device.hpp>

// 3rd party includes
#include <easylogging++.h>

using namespace Devices;

const std::string TestDevice::TEST_DEVICE_TYPE_NAME = "Test Device";

TestDevice::TestDevice() : Device(DeviceType::UNSPECIFIED) {
  this->deviceState = DeviceStatus::UNKNOWN_DEVICE_STATUS;
}

TestDevice::~TestDevice() {}

std::string TestDevice::getDeviceTypeName() {
  return TestDevice::TEST_DEVICE_TYPE_NAME;
}

bool TestDevice::specificWrite(std::shared_ptr<WriteDeviceMessage> writeMsg) {
  this->pushMessageQueue(std::shared_ptr<DeviceMessage>(new ReadDeviceMessage(
      this->getUserId(), writeMsg->getSource(),
      ReadDeviceTopic::READ_TOPIC_DEVICE_SPECIFIC_MSG,
      new GenericReadPayload(std::vector<unsigned char>{1, 2, 3}), writeMsg)));

  return true;
}

std::list<std::shared_ptr<DeviceMessage>>
TestDevice::specificRead(TimePoint timestamp) {
  return std::list<std::shared_ptr<DeviceMessage>>();
}

bool TestDevice::handleResponse(std::shared_ptr<ReadDeviceMessage> response) {

  if (DeviceStatus::BUSY == this->deviceState) {
    auto statusPayload =
        dynamic_pointer_cast<StatusPayload>(response->getReadPaylod());
    if (statusPayload) {
      if (statusPayload->getDeviceName() == TestDevice::TEST_DEVICE_TYPE_NAME) {
        LOG(INFO) << "Found the remote test device!";
        this->deviceState = DeviceStatus::OPERATING;
        this->remoteTestDeviceId = response->getSource();
        this->pushMessageQueue(
            std::shared_ptr<DeviceMessage>(new WriteDeviceMessage(
                this->getUserId(), this->remoteTestDeviceId,
                WriteDeviceTopic::WRITE_TOPIC_DEVICE_SPECIFIC)));
      } else {
        LOG(INFO) << "The responding device is not an test device.";
      }
    } else {
      LOG(INFO) << "Unsolicited response received.";
      return false;
    }
  }

  else if (DeviceStatus::OPERATING == this->deviceState) {
    auto responsePayload =
        dynamic_pointer_cast<GenericReadPayload>(response->getReadPaylod());
    if (responsePayload) {
      LOG(INFO) << "Got response.";
      this->receivedVector = responsePayload->getByteVector();
      return true;
    } else {
      LOG(WARNING) << "Got response, but it was malformed";
      return false;
    }
  }

  return false;
}

bool TestDevice::configure(
    std::shared_ptr<ConfigurationPayload> deviceConfiguration) {
  return true;
}

bool TestDevice::initialize(std::shared_ptr<InitPayload> initPayload) {
  return true;
}

bool TestDevice::start() {
  // Find the local network worker.
  std::list<std::shared_ptr<StatusPayload>> statusPayloads =
      this->messageDistributor->getStatus();
  bool proxyIdsFound = false;
  std::list<UserId> remoteIds;
  for (auto statusPayload : statusPayloads) {
    if (statusPayload->getDeviceName() == Core::NETWORK_WORKER_TYPE_NAME) {
      // Get the ids of the other end point.
      proxyIdsFound = true;
      remoteIds = statusPayload->getProxyIds();
    }
  }
  if (!proxyIdsFound) {
    LOG(ERROR) << "No remote message interfaces found. Can not start.";
    return true;
  }
  // Send status request to the remote ids.
  for (auto &remoteId : remoteIds) {
    this->pushMessageQueue(std::shared_ptr<DeviceMessage>(
        new WriteDeviceMessage(this->getUserId(), remoteId,
                               WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));
  }
  this->deviceState = DeviceStatus::BUSY;

  return true;
}

bool TestDevice::stop() { return true; }

std::vector<unsigned char> TestDevice::returnReceivedVector() {
  return this->receivedVector;
}

std::string TestDevice::getDeviceSerialNumber() { return "Test Device"; }
