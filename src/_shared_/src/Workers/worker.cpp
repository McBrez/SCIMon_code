// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <worker.hpp>

namespace Workers {

Worker::Worker() {}

Worker::~Worker() {}

bool Worker::write(shared_ptr<WriteDeviceMessage> writeMsg) {

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

  else {
    // Could not identify the topic of the message. Return false.
    LOG(ERROR) << "Could not identify the topic of the message.";
    return false;
  }
}

list<shared_ptr<DeviceMessage>> Worker::read(TimePoint timestamp) {
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

bool Worker::write(shared_ptr<InitDeviceMessage> initMsg) {
  if (initMsg->getTargetUserId() != this->getUserId()) {
    LOG(WARNING) << "Got a message that is not meant for this device.";
    return false;
  }

  return false;
}

bool Worker::write(shared_ptr<ConfigDeviceMessage> configMsg) { return false; }

bool Worker::start() { return false; }

bool Worker::stop() { return false; }

} // namespace Workers