// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <message_interface.hpp>

namespace Messages {

MessageInterface::MessageInterface()
    : id(UserId()), messageDistributor(nullptr) {}

UserId MessageInterface::getUserId() const { return this->id; }

bool MessageInterface::operator==(MessageInterface &other) {
  return this->getUserId() == other.getUserId();
}

bool MessageInterface::takeMessage(shared_ptr<DeviceMessage> message) {
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

} // namespace Messages