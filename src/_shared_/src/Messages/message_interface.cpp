// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <data_response_payload.hpp>
#include <key_response_payload.hpp>
#include <message_interface.hpp>
#include <request_data_payload.hpp>

namespace Messages {

MessageInterface::MessageInterface(DeviceType deviceType,
                                   unsigned int dataManagerMeasurementLevel,
                                   DataManagerType dataManagerType)
    : id(UserId(this)), messageDistributor(nullptr),
      dataManager(DataManager::getDataManager(dataManagerType)),
      deviceType(deviceType), deviceState(DeviceStatus::UNKNOWN_DEVICE_STATUS),
      dataManagerMeasurementLevel(dataManagerMeasurementLevel) {}

UserId MessageInterface::getUserId() const { return this->id; }

bool MessageInterface::operator==(MessageInterface &other) {
  return this->getUserId() == other.getUserId();
}

bool MessageInterface::takeMessage(std::shared_ptr<DeviceMessage> message) {
  // Try to downcast the message.
  // Is it an init message?
  auto initDeviceMessage = dynamic_pointer_cast<InitDeviceMessage>(message);
  if (initDeviceMessage) {
    // ... it is. Trigger an init call.
    return this->write(initDeviceMessage);
  }

  else {
    // It is not an init message. Is it a config message?
    auto configDeviceMessage =
        dynamic_pointer_cast<ConfigDeviceMessage>(message);
    if (configDeviceMessage) {
      // ... it is. Trigger an config call.
      return this->write(configDeviceMessage);
    }

    else {
      // It is not an init and not a config message. Is it a write message?
      auto writeDeviceMessage =
          dynamic_pointer_cast<WriteDeviceMessage>(message);
      if (writeDeviceMessage) {
        // ... it is. Trigger an write call.
        return this->write(writeDeviceMessage);
      }

      else {
        // It is not an init message, not a config message and not a write
        // message. Is it a response message?
        auto readDeviceMessage =
            dynamic_pointer_cast<ReadDeviceMessage>(message);
        if (readDeviceMessage) {
          // ... it is. Trigger an response call.
          return this->handleResponse(readDeviceMessage);
        } else {
          // Unknown message type. Abort here.
          LOG(WARNING)
              << "Encountered unknown message type. Message will be ignored";
          return false;
        }
      }
    }
  }
}

bool MessageInterface::addProxyId(UserId proxyId) {

  if (this->proxyIds.empty()) {
    // Always add the given id if the std::list is empty.
    this->proxyIds.push_back(proxyId);
    return true;
  } else {
    // If the std::list is not empty, check if the id is already in there.
    auto it = std::find(this->proxyIds.begin(), this->proxyIds.end(), proxyId);

    if (it == this->proxyIds.end()) {
      this->proxyIds.push_back(proxyId);
      return true;
    } else {
      return false;
    }
  }
}

bool MessageInterface::removeProxyId(UserId proxyId) {
  auto it = std::find(this->proxyIds.begin(), this->proxyIds.end(), proxyId);

  if (it != this->proxyIds.end()) {
    this->proxyIds.erase(it);
    return true;
  } else {
    return false;
  }
}

bool MessageInterface::isTarget(UserId id) {
  if (this->getUserId() == id) {
    return true;
  } else {
    auto it = find(this->proxyIds.begin(), this->proxyIds.end(), id);
    if (it == this->proxyIds.end()) {
      return false;
    } else {
      return true;
    }
  }
}

std::list<UserId> MessageInterface::getProxyUserIds() const {
  return this->proxyIds;
}

void MessageInterface::clearProxyIds() { this->proxyIds.clear(); }

bool MessageInterface::isExactTarget(UserId id) { return this->id == id; }

void MessageInterface::pushMessageQueue(std::shared_ptr<DeviceMessage> msg) {
  this->messageOutMutex.lock();
  this->messageOut.push(msg);
  this->messageOutMutex.unlock();
}

void MessageInterface::pushMessageQueue(
    const std::vector<std::shared_ptr<DeviceMessage>> &msg) {
  this->messageOutMutex.lock();
  for (auto &singleMsg : msg) {
    this->messageOut.push(singleMsg);
  }
  this->messageOutMutex.unlock();
}

std::shared_ptr<DeviceMessage> MessageInterface::popMessageQueue() {
  this->messageOutMutex.lock();
  if (!messageQueueEmpty()) {
    std::shared_ptr<DeviceMessage> msg = this->messageOut.front();
    this->messageOut.pop();
    this->messageOutMutex.unlock();

    return msg;
  } else {
    this->messageOutMutex.unlock();
    return std::shared_ptr<DeviceMessage>();
  }
}

bool MessageInterface::messageQueueEmpty() { return this->messageOut.empty(); }

DataManagerType MessageInterface::getDataManagerType() const {
  return this->dataManager->getDataManagerType();
}

bool MessageInterface::onInitialized(const std::string &dataManagerFileName,
                                     const KeyMapping &keyMapping,
                                     const SpectrumMapping &spectrumMapping) {
  bool openSuccess = this->dataManager->open(dataManagerFileName, keyMapping);

  if (!openSuccess) {
    return false;
  }

  for (auto keyValuePair : spectrumMapping) {
    bool setupSpectrumSuccess = this->dataManager->setupSpectrum(
        keyValuePair.first, keyValuePair.second);
    if (!setupSpectrumSuccess) {
      return false;
    }
  }

  return true;
}

bool MessageInterface::onConfigured(const KeyMapping &keyMapping,
                                    const SpectrumMapping &spectrumMapping) {
  // Do some sanity checks.
  if (!this->dataManager->isOpen()) {
    return false;
  }

  for (auto keyValuePair : keyMapping) {
    bool createKeySuccess =
        this->dataManager->createKey(keyValuePair.first, keyValuePair.second);

    // Check where to create the group.
    std::string groupKey;
    for (size_t i = 0; i < this->dataManagerMeasurementLevel; i++) {
      size_t pos = keyValuePair.first.find("/");
      groupKey = keyValuePair.first.substr(0, pos);
    }
    bool createGroupSuccess = this->dataManager->createGroup(
        groupKey, {{DataManager::DATA_MANAGER_DEVICETYPE_ATTR_NAME,
                    static_cast<int>(this->getDeviceType())}});
    if (!createKeySuccess || !createGroupSuccess) {
      return false;
    }
  }
  for (auto keyValuePair : spectrumMapping) {
    bool setupSpectrumSuccess = this->dataManager->setupSpectrum(
        keyValuePair.first, keyValuePair.second);
    if (!setupSpectrumSuccess) {
      return false;
    }
  }

  return true;
}

int MessageInterface::removeFromMessageQueue(
    const std::vector<UserId> &destinations) {

  this->messageOutMutex.lock();
  size_t counter = 0;
  std::queue<std::shared_ptr<DeviceMessage>> tempQueue;
  while (!this->messageOut.empty()) {
    auto msg = this->messageOut.front();
    this->messageOut.pop();

    auto it = std::find(destinations.begin(), destinations.end(),
                        msg->getDestination());

    if (it != destinations.end()) {
      tempQueue.push(msg);
    } else {
      counter++;
    }
  }
  this->messageOut.swap(tempQueue);

  this->messageOutMutex.unlock();

  return counter;
}

bool MessageInterface::write(std::shared_ptr<WriteDeviceMessage> writeMsg) {

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
    // Put the device state into the message queue.
    this->pushMessageQueue(std::shared_ptr<DeviceMessage>(new ReadDeviceMessage(
        this->self->getUserId(), writeMsg->getSource(),
        READ_TOPIC_DEVICE_STATUS,
        new StatusPayload(this->getUserId(), this->getDeviceStatus(),
                          this->getProxyUserIds(), this->getDeviceType(),
                          this->getDeviceTypeName(), this->initPayload,
                          this->configPayload),
        writeMsg)));
    return true;
  }

  else if (WriteDeviceTopic::WRITE_TOPIC_REQUEST_DATA == writeMsg->getTopic()) {
    // Data is requested. Try to cast down the payload.
    auto requestedDataPayload =
        dynamic_pointer_cast<RequestDataPayload>(writeMsg->getPayload());
    if (!requestedDataPayload) {
      LOG(ERROR)
          << "Received malformed request data message. This will be ignored.";
      return false;
    }

    // Ask the data manager for the requested data.
    std::vector<TimePoint> timestamps;
    std::vector<Value> values;
    bool readSuccess = this->dataManager->read(
        requestedDataPayload->from, requestedDataPayload->to,
        requestedDataPayload->key, timestamps, values);

    if (!readSuccess) {

      LOG(ERROR) << "Read from data manager was not successfull.";
      return false;
    }

    // Construct payloads from the gathered data.
    std::vector<DataResponsePayload *> dataResponsePayloads =
        DataResponsePayload::constructDataResponsePayload(
            requestedDataPayload->from, requestedDataPayload->to,
            requestedDataPayload->key, timestamps, values);
    // Construct messages from the payloads.
    std::vector<std::shared_ptr<DeviceMessage>> dataResponseMessages;
    dataResponseMessages.reserve(dataResponsePayloads.size());
    for (auto responsePayload : dataResponsePayloads) {
      dataResponseMessages.emplace_back(std::shared_ptr<DeviceMessage>(
          new ReadDeviceMessage(this->getUserId(), writeMsg->getSource(),
                                ReadDeviceTopic::READ_TOPIC_DATA_RESPONSE,
                                responsePayload, writeMsg)));
    }
    this->pushMessageQueue(dataResponseMessages);

    return true;
  }

  else if (WriteDeviceTopic::WRITE_TOPIC_REQUEST_KEYS == writeMsg->getTopic()) {

    KeyResponsePayload *keyResponsePayload =
        new KeyResponsePayload(this->dataManager->getKeyMapping(),
                               this->dataManager->getSpectrumMapping(),
                               this->dataManager->getTimerangeMapping());

    std::shared_ptr<DeviceMessage> responseMsg(
        new ReadDeviceMessage(this->getUserId(), writeMsg->getSource(),
                              ReadDeviceTopic::READ_TOPIC_KEY_RESPONSE,
                              keyResponsePayload, writeMsg));

    this->pushMessageQueue(responseMsg);

    return true;

  }

  else {
    // Could not identify the topic of the message. Return false.
    LOG(ERROR) << "Could not identify the topic of the message.";
    return false;
  }
}

bool MessageInterface::write(std::shared_ptr<InitDeviceMessage> initMsg) {
  if (initMsg->getDestination() != this->getUserId()) {
    LOG(WARNING) << "Message Interface received an unsolicited init "
                    "message. This will be ignored.";
    return false;
  }

  this->initPayload = initMsg->returnPayload();
  return this->initialize(initMsg->returnPayload());
}

bool MessageInterface::write(std::shared_ptr<ConfigDeviceMessage> configMsg) {

  if (configMsg->getDestination() != this->getUserId()) {
    LOG(WARNING) << "Message Interface received an unsolicited configuration "
                    "message. This will be ignored.";

    return false;
  }

  this->configPayload = configMsg->getConfiguration();
  this->eventResponseId = configMsg->getResponseIds();

  return this->configure(configMsg->getConfiguration());
}

std::list<std::shared_ptr<DeviceMessage>>
MessageInterface::read(TimePoint timestamp) {
  // Call the device-specific read operation.
  std::list<std::shared_ptr<DeviceMessage>> readMessages =
      this->specificRead(timestamp);
  // Append the message, if it was not empty.
  if (!readMessages.empty()) {
    for (auto &message : readMessages) {
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

DeviceStatus MessageInterface::getDeviceStatus() { return this->deviceState; }

DeviceType MessageInterface::getDeviceType() { return this->deviceType; }

bool MessageInterface::isConfigured() {
  return this->deviceState == DeviceStatus::IDLE;
}

bool MessageInterface::isInitialized() {
  return this->deviceState == DeviceStatus::INITIALIZED;
}

bool handshake(std::shared_ptr<HandshakeMessage> msg) {
  LOG(WARNING) << "Device does not react to handshake messages.";

  return false;
}

} // namespace Messages
