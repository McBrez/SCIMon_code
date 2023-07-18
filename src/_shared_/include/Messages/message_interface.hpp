
#ifndef MESSAGE_INTERFACE_HPP
#define MESSAGE_INTERFACE_HPP

// Standard includes
#include <chrono>
#include <list>
#include <memory>
#include <queue>

// Project includes
#include <config_device_message.hpp>
#include <data_manager.hpp>
#include <handshake_message.hpp>
#include <init_device_message.hpp>
#include <read_device_message.hpp>
#include <user_id.hpp>
#include <utilities.hpp>
#include <write_device_message.hpp>

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
   * @param dataManagerType The type of the underlying data manager, that shall
   * be used.
   */
  MessageInterface(
      DataManagerType dataManagerType = DataManagerType::DATAMANAGER_TYPE_HDF);

  /**
   * @brief Writes an initialization message to the device.
   * @param initMsg The initialization message. that shall be written to the
   * device.
   * @return True if the initialization message has been received successfully.
   * False otherwise.
   */
  virtual bool write(std::shared_ptr<InitDeviceMessage> initMsg) = 0;

  /**
   * @brief Writes a configuration message to the device.
   * @param configMsg The configuration message. that shall be written to the
   * device.
   * @return True if the configuration message has been received successfully.
   * False otherwise.
   */
  virtual bool write(std::shared_ptr<ConfigDeviceMessage> configMsg) = 0;

  /**
   * @brief Writes a message to the device.
   * @param writeMsg The message that shall be written to the device.
   * @return True if successful. False otherwise.
   */
  virtual bool write(std::shared_ptr<WriteDeviceMessage> writeMsg) = 0;

  /**
   * @brief Writes a handshake message to the device.
   * @param writeMsg The handshake message that shall be written to the device.
   * @return True if successful. False otherwise.
   */
  virtual bool write(std::shared_ptr<HandshakeMessage> writeMsg) = 0;

  /**
   * @brief Handles a device specific message. Called by
   * write std::shared_ptr<WriteDeviceMessage>), if the mssage could not be
   * resolved.
   * @param writeMsg The device specific message.
   * @return True if succesful. False otherwise.
   */
  virtual bool specificWrite(std::shared_ptr<WriteDeviceMessage> writeMsg) = 0;

  /**
   * @brief Reads all messages from the message queue.
   * @param timestamp The time at which this method is called.
   * @return std::list of references to the messages from the message queue. May
   * return an empty std::list, if there was nothing to read.
   */
  virtual std::list<std::shared_ptr<DeviceMessage>>
  read(TimePoint timestamp) = 0;

  /**
   * @brief Device-specific read operation, that is executed on each call of
   * read(). Appends to the outgoing message queue.
   * @param timestamp The time at which this method is called.
   * @return A read message.
   */
  virtual std::list<std::shared_ptr<DeviceMessage>>
  specificRead(TimePoint timestamp) = 0;

  /**
   * @brief Handles the response to a write message that has been sent by this
   * object.
   * @param response The response to a write message that has been sent earlier.
   * @return TRUE if the response has been handled successfully. False
   * otherwise.
   */
  virtual bool handleResponse(std::shared_ptr<ReadDeviceMessage> response) = 0;

  /**
   * @brief Takes the given message and handles it.
   * @param message The message that shall be handled.
   * @return TRUE if the message was handled successfully. FALSE otherwise.
   */
  bool takeMessage(std::shared_ptr<DeviceMessage> message);

  /**
   * @brief Returns the unique id of the object, that implements this interface.
   * @return The unique id of the object, that implements this interface.
   */
  UserId getUserId() const;

  /**
   * @brief Returns the Ids this object is the proxy of.
   * @return The std::list of proxy ids of this object.
   */
  std::list<UserId> getProxyUserIds() const;

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
   * @brief Returns whether the given id identifies this object. An id
   * identifies an object if the given id is identical to the primary id of the
   * object.
   * @param id The id that shall be checked for.
   * @return TRUE if the given id identifies this object. FALSE otherwise.
   */
  bool isExactTarget(UserId id);

  /**
   * @brief Constructs the current status of the object.
   * @return Pointer to the current status of the object.
   */
  virtual std::shared_ptr<StatusPayload> constructStatus() = 0;

  /**
   * @brief Returns the used data manager type.
   * @return The used data manager type.
   */
  DataManagerType getDataManagerType() const;

protected:
  /// Reference to the message distributor this object belongs to. Is set when
  /// the message interface object is added to the distributor as participant.
  MessageDistributor *messageDistributor;

  /// A sharable reference to the object itself. Is set when
  /// the message interface object is added to the distributor as participant.
  std::shared_ptr<MessageInterface> self;

  /**
   * @brief Adds the given proxy id to the internal std::list of proxy ids of
   * this object.
   * @param proxyId The proxy id that shall be added.
   * @return True if the id has been added. False otherwise.
   */
  bool addProxyId(UserId proxyId);

  /**
   * @brief Removes the given proxy id from the internal std::list of proxy ids.
   * @param proxyId The proxy ids that shall be removed.
   * @return True if the id has been removed. False otherwise.
   */
  bool removeProxyId(UserId proxyId);

  /**
   * @brief Clear the internal std::list of proxy ids.
   */
  void clearProxyIds();

  /**
   * @brief Pushes the given message to the outgoing queue.
   * @param msg The message that shall be pushed to the outgoing message queue.
   */
  void pushMessageQueue(std::shared_ptr<DeviceMessage> msg);

  /**
   * @brief Pushes multiple messages to the outgoing queue.
   * @param msg The messages that shall be pushed to the outgoing message queue.
   */
  void pushMessageQueue(const std::vector<std::shared_ptr<DeviceMessage>> &msg);

  /**
   * @brief Pops a message from the internal message queue.
   * @return Pointer to a message. May be a nullptr, if there was no message in
   * the queue.
   */
  std::shared_ptr<DeviceMessage> popMessageQueue();

  /**
   * @brief Indicates, whether the internal message queue is empty.
   * @return True if internal message queue is empty. False otherwise.
   */
  bool messageQueueEmpty();

private:
  /// The unique id of the object that implements this interface.
  UserId id;

  /// The unique ids of the objects that are represented by this object.
  std::list<UserId> proxyIds;

  /// Queue for outgoing messages.
  std::queue<std::shared_ptr<DeviceMessage>> messageOut;

  /// Mutex that guards the messageOut queue.
  std::mutex messageOutMutex;

  /// Pointer to the data manager.
  std::unique_ptr<DataManager> dataManager;

  /// The data manager type , that is used by the message interface.
  DataManagerType dataManagerType;
};

} // namespace Messages

#endif
