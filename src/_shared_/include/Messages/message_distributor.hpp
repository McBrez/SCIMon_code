#ifndef MESSAGE_DISTRIBUTOR_HPP
#define MESSAGE_DISTRIBUTOR_HPP

// Standard includes
#include <list>
#include <map>

// Project includes
#include <message_interface.hpp>

namespace Messages {
/// Maps from a single message interface (subscriber) to a list of message
/// interfaces (subscribees).
using SubscriberMapping =
    map<shared_ptr<MessageInterface>, list<shared_ptr<MessageInterface>>>;

class MessageDistributor {
public:
  bool subscribe(shared_ptr<MessageInterface> subscriber,
                 shared_ptr<MessageInterface> target);

  bool unsubscribe(shared_ptr<MessageInterface> subscriber,
                   shared_ptr<MessageInterface> target);

  bool takeMessage(shared_ptr<MessageInterface> sender,
                   shared_ptr<DeviceMessage> message);
  bool takeMessage(shared_ptr<MessageInterface> sender,
                   list<shared_ptr<DeviceMessage>> message);

  bool deliverMessages();

  list<shared_ptr<MessageInterface>> getSubscribers();

private:
  SubscriberMapping subscribers;
};
}; // namespace Messages

#endif