// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <device.hpp>
#include <status_payload.hpp>

namespace Devices {
Device::Device()
    : configurationFinished(false), initFinished(false),
      deviceState(DeviceStatus::UNKNOWN_DEVICE_STATUS) {}

Device::~Device() {}

DeviceId Device::getDeviceId() { return this->deviceId; }

DeviceStatus Device::getDeviceStatus() { return this->deviceState; }

bool Device::isConfigured() { return this->configurationFinished; }

bool Device::isInitialized() { return this->initFinished; }

list<shared_ptr<DeviceMessage>> Device::readN(unsigned int n) {
  return list<shared_ptr<DeviceMessage>>();
}

bool Device::write(shared_ptr<WriteDeviceMessage> writeMsg) {

  if (WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE == writeMsg->getTopic()) {
    this->messageOut.push(shared_ptr<ReadDeviceMessage>(new ReadDeviceMessage(
        ReadDeviceTopic::READ_TOPIC_DEVICE_STATUS,
        new StatusPayload(this->deviceId, this->deviceState), writeMsg)));
  }

  else if (WriteDeviceTopic::WRITE_TOPIC_INVALID == writeMsg->getTopic()) {
    LOG(WARNING) << "Received a message with invalid topic.";
    return false;
  }

  else if (WriteDeviceTopic::WRITE_TOPIC_DEVICE_SPECIFIC ==
           writeMsg->getTopic()) {
    // Message topic is too specific. Pass it to the sub class.
    this->specificWrite(writeMsg);
  }

  else {
    // Could not identify the topic of the message. Return false.
    LOG(ERROR) << "Could not identify the topic of the message.";
    return false;
  }
}

shared_ptr<ReadDeviceMessage> Device::read() {
  // Call the device-specific read operation.
  shared_ptr<ReadDeviceMessage> readMsg = this->specificRead();

  // Append the message, if it was not empty.
  if (readMsg) {
    this->messageOut.push(readMsg);
  }

  // Pop the queue, if it is not empty.
  if (this->messageOut.empty()) {
    return shared_ptr<ReadDeviceMessage>();
  } else {

    shared_ptr<ReadDeviceMessage> retVal = this->messageOut.front();
    this->messageOut.pop();
    return retVal;
  }
}

} // namespace Devices