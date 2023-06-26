#ifndef CONTROL_WORKER_HPP
#define CONTROL_WORKER_HPP

// Standard includes
#include <map>

// Project includes
#include <sentry_config_payload.hpp>
#include <sentry_init_payload.hpp>
#include <worker.hpp>

using namespace Devices;

namespace Workers {

/**
 * @brief Identfies the sub states of the control worker.
 */
enum ControlWorkerSubState {
  /// Invalid sub state.
  CONTROL_WORKER_SUBSTATE_IVALID = 0x00,
  /// It is waited for the connect command
  CONTROL_WORKER_SUBSTATE_WAITING_FOR_CONNECTION = 0x01,
  /// Control worker is connecting
  CONTROL_WORKER_SUBSTATE_CONNECTING = 0x02,
  /// Control worker is connected.
  CONTROL_WORKER_SUBSTATE_CONNECTED = 0x03,
  /// Control worker is exploring the remote host.
  CONTROL_WORKER_SUBSTATE_CONF_EXPLORE = 0x04,
  /// Control worker is initializing the remote host.
  CONTROL_WORKER_SUBSTATE_INIT_REMOTE = 0x05,
  /// Control worker is configuring the remote host.
  CONTROL_WORKER_SUBSTATE_CONF_REMOTE = 0x06,
  /// Remote host is currently running.
  CONTROL_WORKER_SUBSTATE_REMOTE_RUNNING = 0x07,
  /// Remot host is currently stopped.
  CONTROL_WORKER_SUBSTATE_REMOTE_STOPPED = 0x08
};

/**
 * @brief Worker that interfaces with the UI part of SCIMon.
 */
class ControlWorker : public Worker {
public:
  ControlWorker();

  virtual ~ControlWorker() override;

  virtual void work(TimePoint timestamp) override;

  virtual bool start() override;

  virtual bool stop() override;

  virtual bool initialize(std::shared_ptr<InitPayload> initPayload) override;

  virtual bool
  configure(std::shared_ptr<ConfigurationPayload> configPayload) override;

  /**
   * @brief Writes a handshake message to the device.
   * @param writeMsg The handshake message that shall be written to the device.
   * @return True if successful. False otherwise.
   */
  virtual bool write(std::shared_ptr<HandshakeMessage> writeMsg) override;

  /**
   * @brief Handles a device specific message. Called by
   * write std::shared_ptr<WriteDeviceMessage>), if the mssage could not be
   * resolved.
   * @param writeMsg The device specific message.
   * @return True if succesful. False otherwise.
   */
  virtual bool
  specificWrite(std::shared_ptr<WriteDeviceMessage> writeMsg) override;

  /**
   * @brief Device-specific read operation, that is executed on each call of
   * read(). Appends to the outgoing message queue.
   * @param timestamp The time at which this method is called.
   * @return A read message.
   */
  virtual std::list<std::shared_ptr<DeviceMessage>>
  specificRead(TimePoint timestamp) override;

  /**
   * @brief Handles the response to a write message that has been sent by this
   * object.
   * @param response The response to a write message that has been sent earlier.
   * @return TRUE if the response has been handled successfully. False
   * otherwise.
   */
  virtual bool
  handleResponse(std::shared_ptr<ReadDeviceMessage> response) override;

  /**
   * @brief Returns the name of the worker.
   * @return A string that identifies the type of the worker.
   */
  virtual std::string getWorkerName() override;

  /**
   * @brief Try to connect to the specified host.
   * @param ip The IP to which a connection shall be established.
   * @param port The port to which a connection shall be established.
   * @return TRUE if connection was successfull. FALSE otherwise.
   */
  bool startConnect(std::string ip, int port);

  /**
   * @brief Starts the configuration of the control worker.
   * I.e. The control worker queries all remote message
   * interface object.
   * @return
   */
  bool startConfig(std::shared_ptr<SentryInitPayload> initPayload,
                   std::shared_ptr<SentryConfigPayload> configPayload);

  ControlWorkerSubState getControlWorkerState() const;

  /// Identifies a device in the remoteHostIds mapping, whose name is not yet
  /// known.
  const static std::string UnknownDeviceSpecifier;

  /**
   * @brief Periodically queries the state of remote devices.
   */
  void queryRemoteStateWorker();

  /**
   * @brief Returns the state of the remote workers.
   * @return A list containing the states of the remote workers.
   */
  std::list<std::shared_ptr<StatusPayload>> getRemoteStatus();

private:
  /**
   * @brief Returns the User id of the remote sentry from the remote id mapper.
   * @return The user id of the remote sentry worker. May return invalid user
   * id, if id of the sentry worker has not yet been resolved.
   */
  UserId getSentryId();

  /**
   * @brief Checks if the given read device message contains a status payload
   * and caches the status payloads, that are received due to the periodic
   * status queries from the query state worker.
   * @param sourceId The id of the source device.
   * @param statusPayload The corresponding status payload.
   * @return TRUE if the message contained a status payload and if has been
   * handled. FALSE otherwise.
   */
  bool handleStatusPayload(std::shared_ptr<ReadDeviceMessage> msg);

  /// The id of the network worker.
  UserId networkWorkerId;
  /// The currently active sub state.
  ControlWorkerSubState controlWorkerSubState;
  /// Holds the time point the worker started connecting. Used for time out
  /// detection.
  TimePoint connectionTimeout;
  /// Holds the remote worker/device ids. Maps from user id to a tuple
  /// containing worker/device name, the device type and a flag that indicates,
  /// whether the device name and type have already been resolved..
  std::map<size_t, std::tuple<bool, std::string, DeviceType>> remoteHostIds;
  /// The init payload, that shall be sent to the remote host, during
  /// configuration.
  std::shared_ptr<SentryInitPayload> remoteInitPayload;
  /// The config payload, that shall be sent to the remote host, during
  /// configuration.
  std::shared_ptr<SentryConfigPayload> remoteConfigPayload;
  /// Execution flag for the queryRemoteStateWorker.
  bool doQueryState;
  /// Pointer to the queryRemoteStateWorker thread.
  std::unique_ptr<std::thread> queryRemoteStateThread;
  /// List containing pointers to the remote status.
  std::list<std::shared_ptr<StatusPayload>> remoteStatus;
};
} // namespace Workers

#endif
