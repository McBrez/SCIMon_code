#ifndef MESSAGE_DISTRIBUTOR_HPP
#define MESSAGE_DISTRIBUTOR_HPP

// Standard includes
#include <list>
#include <map>

// Project includes
#include <message_interface.hpp>

namespace Messages {
/// Maps from a single message interface (subscriber) to a list of message
/// interfaces (targets).
using SubscriberMapping =
    map<shared_ptr<MessageInterface>, list<shared_ptr<MessageInterface>>>;

class MessageDistributor {
public:
  /**
   * @brief Subscribes the subscriber to the given target.
   * @param subscriber The subscriber
   * @param target The target.
   * @return True if the target could be subscribed to. False otherwise.
   */
  bool subscribe(shared_ptr<MessageInterface> subscriber,
                 shared_ptr<MessageInterface> target);

  /**
   * @brief Unsubscribes the subscriber from the given target.
   * @param subscriber The subscriber.
   * @param target The target.
   * @return True if the target could be unsubscribed from. False otherwise.
   */
  bool unsubscribe(shared_ptr<MessageInterface> subscriber,
                   shared_ptr<MessageInterface> target);

  void takeMessage(shared_ptr<DeviceMessage> message);
  void takeMessage(list<shared_ptr<DeviceMessage>> messages);

  int deliverMessages();

  list<shared_ptr<MessageInterface>> getSubscribers();

private:
  /// Holds a mapping from subscribers to their targets.
  SubscriberMapping subscribers;

  /// Chaches messages until the next call to deliverMessages().
  list<shared_ptr<DeviceMessage>> messageCache;
};
}; // namespace Messages

#endif