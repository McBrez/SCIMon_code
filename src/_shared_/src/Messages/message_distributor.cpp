// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <message_distributor.hpp>

namespace Messages {

MessageDistributor::MessageDistributor(int loopInterval)
    : loopInterval(Duration(loopInterval)) {}

void MessageDistributor::takeMessage(shared_ptr<DeviceMessage> message) {
  this->messageCache.push_back(message);
}

void MessageDistributor::takeMessage(list<shared_ptr<DeviceMessage>> messages) {
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
    shared_ptr<MessageInterface> participant) {
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

list<shared_ptr<MessageInterface>> MessageDistributor::getParticipants() {
  return this->participants;
}

void MessageDistributor::run() {
  while (this->doRun) {
    // Get the current time.
    TimePoint now(chrono::system_clock::now());

    // Get all messages from the participants.
    for (auto participant : this->participants) {
      list<shared_ptr<DeviceMessage>> gatheredMessages = participant->read(now);
      this->messageCache.splice(this->messageCache.end(), gatheredMessages);
    }

    // Send messages to recipients.
    for (auto message : this->messageCache) {
      // Find the target participant.
      UserId destination = message->getDestination();
      auto targetParticipant =
          find_if(this->participants.begin(), this->participants.end(),
                  [destination](shared_ptr<MessageInterface> value) {
                    return destination == value->getUserId();
                  });

      if (targetParticipant == this->participants.end()) {
        // Destination does not exist. Log this and add a failed response
        // message to the cache.
        LOG(WARNING) << "Message from " << message->getSource().id()
                     << " could not be delivered. Destination does not exist.";
        this->failedResponseCache.push_back(shared_ptr<DeviceMessage>(
            new ReadDeviceMessage(message->getDestination(),
                                  message->getSource(),
                                  ReadDeviceTopic::READ_TOPIC_FAILED_RESPONSE,
                                  nullptr, shared_ptr<WriteDeviceMessage>())));

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
    TimePoint newNow = chrono::system_clock::now();
    if (nextTimepoint < newNow) {
      LOG(WARNING) << "Can not keep up. Next loop iteration is scheduled at "
                   << nextTimepoint << " but the current time is " << newNow
                   << ".";
    }

    this_thread::sleep_until(nextTimepoint);
  }
}

} // namespace Messages