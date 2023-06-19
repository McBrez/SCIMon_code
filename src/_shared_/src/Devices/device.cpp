// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <device.hpp>
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

bool Device::write(std::shared_ptr<WriteDeviceMessage> writeMsg) {

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
    this->pushMessageQueue(std::shared_ptr<DeviceMessage>(new ReadDeviceMessage(
        this->self->getUserId(), writeMsg->getSource(),
        READ_TOPIC_DEVICE_STATUS,
        new StatusPayload(this->getUserId(), this->getDeviceStatus(),
                          this->getProxyUserIds(), this->getDeviceType(),
                          this->getDeviceTypeName()),
        writeMsg)));
    return true;
  }

  else {
    // Could not identify the topic of the message. Return false.
    LOG(ERROR) << "Could not identify the topic of the message.";
    return false;
  }
}

std::list<std::shared_ptr<DeviceMessage>> Device::read(TimePoint timestamp) {
  // Call the device-specific read operation.
  std::list<std::shared_ptr<DeviceMessage>> readMessages =
      this->specificRead(timestamp);
  // Append the message, if it was not empty.
  if (!readMessages.empty()) {
    for (auto message : readMessages) {
      this->pushMessageQueue(message);
    }
  }

  // Pop the queue, if it is not empty.
  if (this->messageQueueEmpty()) {
    // Queue is empty. Return an empty std::list.
    return std::list<std::shared_ptr<DeviceMessage>>();
  } else {
    // Pop the queue to a std::list and return it.
    std::list<std::shared_ptr<DeviceMessage>> retVal;
    while (!this->messageQueueEmpty()) {
      retVal.push_back(this->popMessageQueue());
    }
    return retVal;
  }
}

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

bool Device::write(std::shared_ptr<InitDeviceMessage> initMsg) {
  if (initMsg->getDestination() != this->getUserId()) {
    LOG(WARNING) << "Got a message that is not meant for this device.";
    return false;
  }

  return this->initialize(initMsg->returnPayload());
}

bool Device::write(std::shared_ptr<ConfigDeviceMessage> configMsg) {

  this->responseId = configMsg->getResponseId();
  return this->configure(configMsg->getConfiguration());
}

bool Device::write(std::shared_ptr<HandshakeMessage> writeMsg) {
  // Devices should not react to handshake messages.
  LOG(WARNING) << "Device received a handshake message. This will be ingnored.";
  return false;
}

DeviceType Device::getDeviceType() { return this->deviceType; }

std::shared_ptr<StatusPayload> Device::constructStatus() {
  return std::shared_ptr<StatusPayload>(new StatusPayload(
      this->getUserId(), this->getDeviceStatus(), this->getProxyUserIds(),
      this->getDeviceType(), this->getDeviceTypeName()));
}

} // namespace Devices
