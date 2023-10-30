// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <common.hpp>
#include <message_distributor.hpp>

namespace Messages {

MessageDistributor::MessageDistributor(int loopInterval)
    : loopInterval(Duration(loopInterval)) {}

MessageDistributor::MessageDistributor(std::chrono::milliseconds loopInterval)
    : loopInterval(loopInterval) {}

void MessageDistributor::takeMessage(std::shared_ptr<DeviceMessage> message) {
  this->messageCache.push_back(message);
}

void MessageDistributor::takeMessage(
    std::list<std::shared_ptr<DeviceMessage>> messages) {
  for (auto message : messages) {
    this->takeMessage(message);
  }
}

int MessageDistributor::deliverMessages() {

  // Deliver the messages with concrete targets.
  for (auto message : this->messageCache) {
    // Does the message have a destination?
  }

  // Deliver the messages without targets.
  return false;
}

bool MessageDistributor::addParticipant(
    std::shared_ptr<MessageInterface> participant) {
  // Check if the given participant is already known.
  auto it = std::find(this->participants.begin(), this->participants.end(),
                      participant);
  if (it != this->participants.end()) {
    // The participant is already known. Abort here.
    return false;
  }

  this->participants.push_back(participant);
  participant->messageDistributor = this;
  participant->self = participant;
  return true;
}

std::list<UserId> MessageDistributor::getParticipants() {
  std::list<UserId> retVal;
  for (auto participant : this->participants) {
    retVal.push_back(participant->getUserId());
  }

  return retVal;
}

std::list<std::shared_ptr<StatusPayload>> MessageDistributor::getStatus() {
  std::list<std::shared_ptr<StatusPayload>> retVal;

  for (auto participant : this->participants) {

    retVal.emplace_back(participant->constructStatus());
  }

  return retVal;
}

void MessageDistributor::run() {
  this->doRun = true;
  while (this->doRun) {
    // Get the current time.
    TimePoint now(Core::getNow());

    // Get all messages from the participants.
    for (auto participant : this->participants) {
      std::list<std::shared_ptr<DeviceMessage>> gatheredMessages =
          participant->read(now);
      this->messageCache.splice(this->messageCache.end(), gatheredMessages);
    }

    // Send messages to recipients.
    for (auto message : this->messageCache) {
      // Find the target participant.
      UserId destination = message->getDestination();
      auto targetParticipant =
          find_if(this->participants.begin(), this->participants.end(),
                  [destination](std::shared_ptr<MessageInterface> value) {
                    return value->isTarget(destination);
                  });

      if (targetParticipant == this->participants.end()) {
        // Destination does not exist. Log this and add a failed response
        // message to the cache.
        LOG(WARNING) << "Message from " << message->getSource().id()
                     << " could not be delivered. Destination does not exist.";
        this->failedResponseCache.push_back(
            std::shared_ptr<DeviceMessage>(new ReadDeviceMessage(
                message->getDestination(), message->getSource(),
                ReadDeviceTopic::READ_TOPIC_FAILED_RESPONSE, nullptr,
                std::shared_ptr<WriteDeviceMessage>())));

      } else {
        // Destination found. Let the object take the message.
        bool processSuccess = (*targetParticipant)->takeMessage(message);

        if (!processSuccess) {
          LOG(WARNING) << "Targeted participant  "
                       << message->getDestination().id()
                       << " was not able to process the message.";
        }
      }
    }

    // All messages have been delivered. Clear the cache.
    this->messageCache.clear();
    // Add the responses that failed to the cache, so that they are handled in
    // the next loop iteration.
    this->messageCache.splice(messageCache.end(), this->failedResponseCache);

    TimePoint nextTimepoint = now + this->loopInterval;
    TimePoint newNow = Core::getNow();
    if (nextTimepoint < newNow) {
      LOG(WARNING) << "Can not keep up. Next loop iteration is scheduled at "
                   << nextTimepoint << " but the current time is " << newNow
                   << ".";
    }

    std::this_thread::sleep_until(nextTimepoint);
  }
}

void MessageDistributor::stop() { this->doRun = false; }

bool MessageDistributor::isRunning() const { return this->doRun; }

} // namespace Messages
