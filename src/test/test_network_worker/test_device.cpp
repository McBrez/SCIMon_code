// Project includes
#include <common.hpp>
#include <generic_read_payload.hpp>
#include <message_distributor.hpp>
#include <test_device.hpp>

// 3rd party includes
#include <easylogging++.h>

using namespace Devices;

const string TestDevice::TEST_DEVICE_TYPE_NAME = "Test Device";

TestDevice::TestDevice() : Device(DeviceType::UNSPECIFIED) {
  this->deviceState = DeviceStatus::UNKNOWN_DEVICE_STATUS;
}

TestDevice::~TestDevice() {}

string TestDevice::getDeviceTypeName() {
  return TestDevice::TEST_DEVICE_TYPE_NAME;
}

bool TestDevice::specificWrite(shared_ptr<WriteDeviceMessage> writeMsg) {
  this->messageOut.push(shared_ptr<DeviceMessage>(new ReadDeviceMessage(
      this->getUserId(), writeMsg->getSource(),
      ReadDeviceTopic::READ_TOPIC_DEVICE_SPECIFIC_MSG,
      new GenericReadPayload(vector<unsigned char>{1, 2, 3}), writeMsg)));

  return false;
}

list<shared_ptr<DeviceMessage>> TestDevice::specificRead(TimePoint timestamp) {
  return list<shared_ptr<DeviceMessage>>();
}

bool TestDevice::handleResponse(shared_ptr<ReadDeviceMessage> response) {

  if (DeviceStatus::BUSY == this->deviceState) {
    auto statusPayload =
        dynamic_pointer_cast<StatusPayload>(response->getReadPaylod());
    if (statusPayload) {
      if (statusPayload->getDeviceName() == TestDevice::TEST_DEVICE_TYPE_NAME) {
        LOG(INFO) << "Found the remote test device!";
        this->deviceState = DeviceStatus::OPERATING;
        this->remoteTestDeviceId = statusPayload->getDeviceId();
        this->messageOut.push(shared_ptr<DeviceMessage>(new WriteDeviceMessage(
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
}

bool TestDevice::configure(
    shared_ptr<ConfigurationPayload> deviceConfiguration) {
  return true;
}

bool TestDevice::initialize(shared_ptr<InitPayload> initPayload) {
  return true;
}

bool TestDevice::start() {
  // Find the local network worker.
  list<shared_ptr<StatusPayload>> statusPayloads =
      this->messageDistributor->getStatus();
  bool proxyIdsFound = false;
  list<UserId> remoteIds;
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
  for (auto remoteId : remoteIds) {
    this->messageOut.push(shared_ptr<DeviceMessage>(
        new WriteDeviceMessage(this->getUserId(), remoteId,
                               WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));
  }
  this->deviceState = DeviceStatus::BUSY;
}

bool TestDevice::stop() { return true; }

vector<unsigned char> TestDevice::returnReceivedVector() {
  return this->receivedVector;
}
