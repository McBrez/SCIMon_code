// Project includes
#include <message_distributor.hpp>

namespace Messages {

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
  return true;
}

list<shared_ptr<MessageInterface>> MessageDistributor::getParticipants() {
  return this->participants;
}

} // namespace Messages