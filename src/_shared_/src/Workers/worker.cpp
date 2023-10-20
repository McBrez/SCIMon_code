// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <worker.hpp>

namespace Workers {

WorkerId Worker::workerIdCounter = 0;

Worker::Worker(Utilities::DataManagerType dataManagerType)
    : MessageInterface(dataManagerType), workerId(++Worker::workerIdCounter),
      workerState(DeviceStatus::UNKNOWN_DEVICE_STATUS) {}

Worker::~Worker() {}

bool Worker::write(std::shared_ptr<WriteDeviceMessage> writeMsg) {

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
    return this->start();
  }

  else if (WriteDeviceTopic::WRITE_TOPIC_STOP == writeMsg->getTopic()) {
    return this->stop();
  }

  else if (WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE == writeMsg->getTopic()) {
    // Put the worker state into the message queue.
    this->pushMessageQueue(std::shared_ptr<DeviceMessage>(new ReadDeviceMessage(
        this->self->getUserId(), writeMsg->getSource(),
        READ_TOPIC_DEVICE_STATUS,
        new StatusPayload(this->getUserId(), this->getState(),
                          this->getProxyUserIds(), DeviceType::UNSPECIFIED,
                          this->getWorkerName(), this->initPayload,
                          this->configPayload),
        writeMsg)));
    return true;
  }

  else {
    // Could not identify the topic of the message. Return false.
    LOG(ERROR) << "Could not identify the topic of the message.";
    return false;
  }
}

std::list<std::shared_ptr<DeviceMessage>> Worker::read(TimePoint timestamp) {
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

bool Worker::write(std::shared_ptr<InitDeviceMessage> initMsg) {
  if (initMsg->getDestination() != this->getUserId()) {
    LOG(WARNING) << "Got a message that is not meant for this device.";
    return false;
  }

  return this->initialize(initMsg->returnPayload());
}

bool Worker::write(std::shared_ptr<ConfigDeviceMessage> configMsg) {
  if (configMsg->getDestination() != this->getUserId()) {
    LOG(WARNING) << "Got a message that is not meant for this device.";
    return false;
  }
  this->eventResponseId = configMsg->getResponseIds();
  return this->configure(configMsg->getConfiguration());
}

DeviceStatus Worker::getState() const { return this->workerState; }

std::shared_ptr<StatusPayload> Worker::constructStatus() {
  return std::shared_ptr<StatusPayload>(new StatusPayload(
      this->getUserId(), this->workerState, this->getProxyUserIds(),
      DeviceType::UNSPECIFIED, this->getWorkerName(), this->initPayload,
      this->configPayload));
}

} // namespace Workers
