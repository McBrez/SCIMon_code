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
  /// Control worker fetches the remote data keys.
  CONTROL_WORKER_SUBSTATE_CONF_GET_DATA_KEYS = 0x07,
  /// Remote host is currently running.
  CONTROL_WORKER_SUBSTATE_REMOTE_RUNNING = 0x08,
  /// Remote host is currently stopped.
  CONTROL_WORKER_SUBSTATE_REMOTE_STOPPED = 0x09
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

  /**
   * @brief Sets the enabled state of an remote device/worker.
   * @param remoteId The id of the remote device/worker.
   * @param state The state that shall be set.
   * @return Whether the corresponding message has been sent.
   */
  bool setRemoteWorkerState(UserId remoteId, bool state);

  /**
   * @brief Returns impedance spectra of the most recent data sereis.
   * @param from Start of the queried time interval.
   * @param to End of the queried time interval.
   * @return A vector of impedance spectra.
   */
  std::vector<std::tuple<TimePoint, ImpedanceSpectrum>>
  getSpectra(TimePoint from, TimePoint to);

  /**
   * @brief Return the name of the device type.
   * @return The device type name.
   */
  virtual std::string getDeviceTypeName() override;

  /**
   * @brief Assemble a local data key. Local data keys refer to the locally
   * saved data manager data that has been generated on a remote message
   * interface.
   * @param userId The user id of the message interface the data has been
   * generated.
   * @param dataKey The data key on the remote and.
   * @return The local data key.
   */
  std::string getLocalDataKey(UserId userId, const std::string &dataKey) const;
  std::string getLocalDataKey(size_t userId, const std::string &dataKey) const;

private:
  /**
   * @brief Returns the User id of the remote sentry from the remote id mapper.
   * @return The user id of the remote sentry worker. May return invalid user
   * id, if id of the sentry worker has not yet been resolved.
   */
  UserId getSentryId();

  /**
   * @brief Returns the User id of the remote pump controller from the remote id
   * mapper.
   * @return The user id of the remote pump controller. May return invalid user
   * id, if id of the pump controller has not yet been resolved.
   */
  UserId getPumpControllerId();

  /**
   * @brief Returns the User id of the remote spectrometer from the remote id
   * mapper.
   * @return The user id of the remote spectrometer. May return invalid user
   * id, if id of the spectrometer has not yet been resolved.
   */
  UserId getSpectrometerId();

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

  /**
   * @brief Handles disconnection events from the network worker.
   * @param readMsg A read message that has been received in the
   * handleResponse() function. It will be checked if the message originated
   * from the network worker, and if its payload indicates a disconnection
   * event. If a disconnection event has been received, the state of the control
   * worker will be adjusted accordingly.
   * @return True if a disconnection event has been detected. False otherwise.
   */
  bool
  handleNetworkWorkerDisconnect(std::shared_ptr<ReadDeviceMessage> readMsg);

  /**
   * @brief Worker that queries data from remote devices periodically.
   */
  void dataQueryWorker();

  /**
   * @brief Helper function that searches for the given user id in the local
   * cache and returns the status of the associated message interface.
   * @param userId The user id that shall be queried.
   * @return The status of the queried device.
   */
  DeviceStatus getStateOfRemoteId(const UserId &userId);

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
  /// Holds the remote data keys. Maps from UserId to a tuple containing data
  /// keys, spectrum mapping and timerang mapping.
  std::map<size_t, std::tuple<KeyMapping, SpectrumMapping, TimerangeMapping>>
      remoteDataKeys;
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
  /// Pointer to the data query thread.
  std::unique_ptr<std::thread> dataQueryThread;
  /// The timestamp of the last data query.
  TimePoint lastDataQuery;
  /// The intervall with which the remote end is queried for data.
  Duration dataQueryInterval;
  /// Execution flag for the remote data remote data query thread.
  bool doQueryData;
};
} // namespace Workers

#endif
