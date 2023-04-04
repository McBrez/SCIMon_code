// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <device.hpp>
#include <device_status_message.hpp>
#include <dummy_message.hpp>
#include <status_payload.hpp>

namespace Devices {

Device::Device(DeviceType deviceType)
    : configurationFinished(false), initFinished(false),
      deviceState(DeviceStatus::UNKNOWN_DEVICE_STATUS), deviceType(deviceType),
      startMessageCache(new Messages::DummyMessage()) {}

Device::~Device() {}

DeviceStatus Device::getDeviceStatus() { return this->deviceState; }

bool Device::isConfigured() { return this->configurationFinished; }

bool Device::isInitialized() { return this->initFinished; }

bool Device::write(shared_ptr<WriteDeviceMessage> writeMsg) {

  if (WriteDeviceTopic::WRITE_TOPIC_INVALID == writeMsg->getTopic()) {
    LOG(WARNING) << "Received a message with invalid topic.";
    return false;
  }

  else if (WriteDeviceTopic::WRITE_TOPIC_DEVICE_SPECIFIC ==
           writeMsg->getTopic()) {
    // Message topic is too specific. Pass it to the sub class.
    return this->specificWrite(writeMsg);
  }

  else if (WriteDeviceTopic::WRITE_TOPIC_RUN == writeMsg->getTopic()) {
    this->startMessageCache = writeMsg;
    return this->start();
  }

  else if (WriteDeviceTopic::WRITE_TOPIC_STOP == writeMsg->getTopic()) {
    return this->stop();
  }

  else if (WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE == writeMsg->getTopic()) {
    // Put the device state into the message queue.
    this->messageOut.push(shared_ptr<DeviceMessage>(new DeviceStatusMessage(
        this->self, writeMsg->getSource(), READ_TOPIC_DEVICE_STATUS,
        new StatusPayload(this->getUserId(), this->getDeviceStatus()), writeMsg,
        this->getDeviceStatus())));
    return true;
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

string Device::deviceStatusToString(DeviceStatus deviceStatus) {

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

bool Device::write(shared_ptr<InitDeviceMessage> initMsg) {
  if (initMsg->getDestination().get() != this) {
    LOG(WARNING) << "Got a message that is not meant for this device.";
    return false;
  }

  return this->initialize(initMsg->returnPayload());
}

/**
 * @brief Writes a message to the device that triggers a configuration.
 * @param configMsg The configuration message.
 * @return True if configuration was succesfull. False otherwise.
 */
bool Device::write(shared_ptr<ConfigDeviceMessage> configMsg) {

  return this->configure(configMsg->getConfiguration());
}

DeviceType Device::getDeviceType() { return this->deviceType; }

} // namespace Devices