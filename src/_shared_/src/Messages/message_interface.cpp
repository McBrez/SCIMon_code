// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <message_interface.hpp>

namespace Messages {

MessageInterface::MessageInterface()
    : id(UserId(this)), messageDistributor(nullptr) {}

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
        // ... it is. Trigger an config call.
        return this->write(writeDeviceMessage);
      }

      else {
        // It is not an init and not a config message. Is it a handshake
        // message?
        auto handshakeMessage = dynamic_pointer_cast<HandshakeMessage>(message);
        if (handshakeMessage) {
          // ... it is. Trigger an config call.
          return this->write(handshakeMessage);
        } else {
          // It is not an init message, not a config message and not a write
          // message. Is it a response message?
          auto readDeviceMessage =
              dynamic_pointer_cast<ReadDeviceMessage>(message);
          if (readDeviceMessage) {
            // ... it is. Trigger an config call.
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

} // namespace Messages
