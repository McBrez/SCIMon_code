// Project includes
#include <message_distributor.hpp>

namespace Messages {

bool MessageDistributor::subscribe(shared_ptr<MessageInterface> subscriber,
                                   shared_ptr<MessageInterface> target) {
  return false;
}

bool MessageDistributor::unsubscribe(shared_ptr<MessageInterface> subscriber,
                                     shared_ptr<MessageInterface> target) {
  return false;
}

bool MessageDistributor::takeMessage(shared_ptr<MessageInterface> sender,
                                     shared_ptr<DeviceMessage> message) {
  return false;
}

bool MessageDistributor::takeMessage(shared_ptr<MessageInterface> sender,
                                     list<shared_ptr<DeviceMessage>> message) {
  return false;
}

bool MessageDistributor::deliverMessages() { return false; }

list<shared_ptr<MessageInterface>> MessageDistributor::getSubscribers() {
  return list<shared_ptr<MessageInterface>>();
}

} // namespace Messages