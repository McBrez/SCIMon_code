#ifndef MESSAGE_DISTRIBUTOR_HPP
#define MESSAGE_DISTRIBUTOR_HPP

// Standard includes
#include <list>
#include <map>

// Project includes
#include <message_interface.hpp>

namespace Messages {

class MessageDistributor {
public:
  /**
   * @brief Construct a new MessageDistributor object
   * @param loopInterval The interval of the run loop.
   */
  MessageDistributor(int loopInterval);

  /**
   * @brief Takes the given message. It will be delivered on the next call of
   * deliverMessages().
   * @param message The message that shall be taken.
   */
  void takeMessage(std::shared_ptr<DeviceMessage> message);
  void takeMessage(std::list<std::shared_ptr<DeviceMessage>> messages);

  /**
   * @brief Delivers the message that have been added prior with calls to
   * takeMessage()
   * @return The count of messages that have been delivered.
   */
  int deliverMessages();

  /**
   * @brief Adds the given object to the participants of the message
   * distributor.
   * @param participant The participant that shall be added.
   * @return TRUE if the participant has been added successfully. FALSE
   * otherwise.
   */
  bool addParticipant(std::shared_ptr<MessageInterface> participant);

  /**
   * @brief Return all the participants of the message interface.
   * @return A std::list of all participants of the message interface.
   */
  std::list<UserId> getParticipants();

  /**
   * @brief Returns the status of all participants.
   * @return std::list containing the status of all participants.
   */
  std::list<std::shared_ptr<StatusPayload>> getStatus();

  /**
   * @brief Starts the execution loop. Note that this method blocks, until it is
   * aborted.
   */
  void run();

  /**
   * @brief Stops the execution loop.
   */
  void stop();

    /**
     * @brief Returns whether the message distributor is currently running.
    */
  bool isRunning() const;

private:
  /// Chaches messages until the next call to deliverMessages().
  std::list<std::shared_ptr<DeviceMessage>> messageCache;

  /// Chaches messages that indicate a failed response until they can be added
  /// to the message cache.
  std::list<std::shared_ptr<DeviceMessage>> failedResponseCache;

  /// std::list of participants.
  std::list<std::shared_ptr<MessageInterface>> participants;

  /// Flag that tells the message distributor to run.
  bool doRun;

  /// The loop interval. In milli seconds.
  Duration loopInterval;
};
}; // namespace Messages

#endif
