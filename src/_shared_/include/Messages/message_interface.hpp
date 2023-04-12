#ifndef MESSAGE_INTERFACE_HPP
#define MESSAGE_INTERFACE_HPP

// Standard includes
#include <chrono>
#include <list>
#include <memory>
#include <queue>

// Project includes
#include <config_device_message.hpp>
#include <handshake_message.hpp>
#include <init_device_message.hpp>
#include <read_device_message.hpp>
#include <user_id.hpp>
#include <utilities.hpp>
#include <write_device_message.hpp>

using namespace std;
using namespace Utilities;

namespace Messages {

class MessageDistributor;

/**
 * @brief Declares an interface that allows reading, writing and
 * subscription to messages.
 */
class MessageInterface {
public:
  friend class MessageDistributor;

  /**
   * @brief Construct the object with a automatically generated user id.
   */
  MessageInterface();

  /**
   * @brief Writes an initialization message to the device.
   * @param initMsg The initialization message. that shall be written to the
   * device.
   * @return True if the initialization message has been received successfully.
   * False otherwise.
   */
  virtual bool write(shared_ptr<InitDeviceMessage> initMsg) = 0;

  /**
   * @brief Writes a configuration message to the device.
   * @param configMsg The configuration message. that shall be written to the
   * device.
   * @return True if the configuration message has been received successfully.
   * False otherwise.
   */
  virtual bool write(shared_ptr<ConfigDeviceMessage> configMsg) = 0;

  /**
   * @brief Writes a message to the device.
   * @param writeMsg The message that shall be written to the device.
   * @return True if successful. False otherwise.
   */
  virtual bool write(shared_ptr<WriteDeviceMessage> writeMsg) = 0;

  /**
   * @brief Writes a handshake message to the device.
   * @param writeMsg The handshake message that shall be written to the device.
   * @return True if successful. False otherwise.
   */
  virtual bool write(shared_ptr<HandshakeMessage> writeMsg) = 0;

  /**
   * @brief Handles a device specific message. Called by
   * write(shared_ptr<WriteDeviceMessage>), if the mssage could not be resolved.
   * @param writeMsg The device specific message.
   * @return True if succesful. False otherwise.
   */
  virtual bool specificWrite(shared_ptr<WriteDeviceMessage> writeMsg) = 0;

  /**
   * @brief Reads all messages from the message queue.
   * @param timestamp The time at which this method is called.
   * @return List of references to the messages from the message queue. May
   * return an empty list, if there was nothing to read.
   */
  virtual list<shared_ptr<DeviceMessage>> read(TimePoint timestamp) = 0;

  /**
   * @brief Device-specific read operation, that is executed on each call of
   * read(). Appends to the outgoing message queue.
   * @param timestamp The time at which this method is called.
   * @return A read message.
   */
  virtual list<shared_ptr<DeviceMessage>> specificRead(TimePoint timestamp) = 0;

  /**
   * @brief Handles the response to a write message that has been sent by this
   * object.
   * @param response The response to a write message that has been sent earlier.
   * @return TRUE if the response has been handled successfully. False
   * otherwise.
   */
  virtual bool handleResponse(shared_ptr<ReadDeviceMessage> response) = 0;

  /**
   * @brief Takes the given message and handles it.
   * @param message The message that shall be handled.
   * @return TRUE if the message was handled successfully. FALSE otherwise.
   */
  bool takeMessage(shared_ptr<DeviceMessage> message);

  /**
   * @brief Returns the unique id of the object, that implements this interface.
   * @return The unique id of the object, that implements this interface.
   */
  UserId getUserId() const;

  /**
   * @brief Returns the Ids this object is the proxy of.
   * @return The list of proxy ids of this object.
   */
  list<UserId> getProxyUserIds() const;

  /**
   * @brief Comparison operator. Two message interface are equal, if their user
   * id is equal.
   * @param other The other message interface.
   * @return TRUE if the two message interface object are equal. FALSE
   * otherwise.
   */
  bool operator==(MessageInterface &other);

  /**
   * @brief Returns whether the given id targets this object. The object is
   * targeted by the id if the id of the object itself matches, or one of the
   * proxy ids, this object is holding, matches.
   * @param id The id that shall be checked for.
   * @return TRUE if the id targets this object. FALSE otherwise.
   */
  bool isTarget(UserId id);

  /**
   * @brief Constructs the current status of the object.
   * @return Pointer to the current status of the object.
   */
  virtual shared_ptr<StatusPayload> constructStatus() = 0;

protected:
  /// Queue for outgoing messages.
  queue<shared_ptr<DeviceMessage>> messageOut;

  /// Reference to the message distributor this object belongs to. Is set when
  /// the message interface object is added to the distributor as participant.
  MessageDistributor *messageDistributor;

  /// A sharable reference to the object itself. Is set when
  /// the message interface object is added to the distributor as participant.
  shared_ptr<MessageInterface> self;

  bool addProxyId(UserId proxyId);

  bool removeProxyId(UserId proxyId);

private:
  /// The unique id of the object that implements this interface.
  UserId id;

  list<UserId> proxyIds;
};
} // namespace Messages

#endif