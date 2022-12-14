// Project includes
#include <message_distributor.hpp>

namespace Messages {

bool MessageDistributor::subscribe(shared_ptr<MessageInterface> subscriber,
                                   shared_ptr<MessageInterface> target) {
  // Is the subscriber already subscribed to the target?
  list<shared_ptr<MessageInterface>> targetList = this->subscribers[subscriber];
  auto it = std::find(targetList.begin(), targetList.end(), target);
  if (it != targetList.end()) {
    // Already subscribed to the target. Return false.
    return false;
  }

  // Subscribe to the target.
  this->subscribers[subscriber].push_back(target);
  return true;
}

bool MessageDistributor::unsubscribe(shared_ptr<MessageInterface> subscriber,
                                     shared_ptr<MessageInterface> target) {

  // Is the subscriber already subscribed to the target?
  list<shared_ptr<MessageInterface>> targetList = this->subscribers[subscriber];
  auto it = std::find(targetList.begin(), targetList.end(), target);
  if (it == targetList.end()) {
    // Target is not subscribed by the subscriber. Return false.
    return false;
  }

  // Unsubscribe the target.
  this->subscribers[subscriber].remove(target);
  return true;
}

void MessageDistributor::takeMessage(shared_ptr<DeviceMessage> message) {
  this->messageCache.push_back(message);
}

void MessageDistributor::takeMessage(list<shared_ptr<DeviceMessage>> messages) {
  for (auto message : messages) {
    this->takeMessage(message);
  }
}

int MessageDistributor::deliverMessages() {

  for (auto message : this->messageCache) {
    // Does the message have a destination?
  }
  return false;
}

list<shared_ptr<MessageInterface>> MessageDistributor::getSubscribers() {
  list<shared_ptr<MessageInterface>> retVal;
  for (auto it : this->subscribers) {
    retVal.push_back(it.first);
  }
  return retVal;
}

} // namespace Messages