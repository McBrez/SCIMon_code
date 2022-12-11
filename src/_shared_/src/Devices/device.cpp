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

DeviceStatus Device::getDeviceStatus() { return this->deviceState; }

bool Device::isConfigured() { return this->configurationFinished; }

bool Device::isInitialized() { return this->initFinished; }

bool Device::write(shared_ptr<WriteDeviceMessage> writeMsg) {

  if (WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE == writeMsg->getTopic()) {
    this->messageOut.push(shared_ptr<ReadDeviceMessage>(new ReadDeviceMessage(
        ReadDeviceTopic::READ_TOPIC_DEVICE_STATUS,
        new StatusPayload(this->getUserId(), this->deviceState), writeMsg)));
  }

  else if (WriteDeviceTopic::WRITE_TOPIC_INVALID == writeMsg->getTopic()) {
    LOG(WARNING) << "Received a message with invalid topic.";
    return false;
  }

  else if (WriteDeviceTopic::WRITE_TOPIC_DEVICE_SPECIFIC ==
           writeMsg->getTopic()) {
    // Message topic is too specific. Pass it to the sub class.
    return this->specificWrite(writeMsg);
  }

  else {
    // Could not identify the topic of the message. Return false.
    LOG(ERROR) << "Could not identify the topic of the message.";
    return false;
  }
}

list<shared_ptr<DeviceMessage>> Device::read(TimePoint timestamp) {
  // Call the device-specific read operation.
  list<shared_ptr<DeviceMessage>> readMessages = this->specificRead(timestamp);
  // Append the message, if it was not empty.
  if (!readMessages.empty()) {
    for (auto message : readMessages) {
      this->messageOut.push(message);
    }
  }

  // Pop the queue, if it is not empty.
  if (this->messageOut.empty()) {
    // Queue is empty. Return an empty list.
    return list<shared_ptr<DeviceMessage>>();
  } else {
    // Pop the queue to a list and return it.
    list<shared_ptr<DeviceMessage>> retVal;

    while (!this->messageOut.empty()) {
      retVal.push_back(this->messageOut.front());
      this->messageOut.pop();
    }
    return retVal;
  }
}

} // namespace Devices