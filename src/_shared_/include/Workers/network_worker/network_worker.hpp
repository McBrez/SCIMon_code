#ifndef NETWORK_WORKER_HPP
#define NETWORK_WORKER_HPP

// Standard includes
#include <mutex>
#include <queue>

// Project includes
#include <message_factory.hpp>
#include <network_worker_init_payload.hpp>
#include <socket_wrapper.hpp>
#include <worker.hpp>

using namespace Utilities;

namespace Workers {

/// Identifies the states of the network communication.
enum NetworkWorkerCommState {
  /// Invalid state.
  NETWORK_WOKER_COMM_STATE_INVALID,
  /// Worker is std::listening to connection requests.
  NETWORK_WOKER_COMM_STATE_LISTENING,
  /// Worker is starting up.
  NETWORK_WOKER_COMM_STATE_STARTING,
  /// When Worker is server, request for handshake is waited for. When worker is
  /// client, handshake is sent.
  NETWORK_WOKER_COMM_STATE_HANDSHAKING,
  /// Worker is waiting for the handshake response.
  NETWORK_WOKER_COMM_STATE_WAITING_FOR_HANDSHAKE_RESPONSE,
  /// Worker is connected and continuosly sending/receiving.
  NETWORK_WOKER_COMM_STATE_WORKING,
  /// Worker encountered an error and is waiting to be initialized again.
  NETWORK_WOKER_COMM_STATE_ERROR
};

/**
 * @brief Encapsulates a worker that allows communication over a socket.
 */
class NetworkWorker : public Worker {
public:
  /**
   * @brief Construct a new Network Worker object
   */
  NetworkWorker();

  /**
   * @brief Destroy the Network Worker object
   */
  virtual ~NetworkWorker() override;

  /**
   * @brief Worker function of the network worker.
   * @param timestamp The timestamp of the time point this function has been
   * called.
   */
  virtual void work(TimePoint timestamp) override;

  /**
   * @brief Starts operation of the network worker.
   * @return TRUE if operation could be started. FALSE otherwise.
   */
  virtual bool start() override;

  /**
   * @brief Stops operation of the network worker.
   * @return TRUE if the operation could be stopped. FALSE otherwise.
   */
  virtual bool stop() override;

  /**
   * @brief Triggers an initialization of the network worker. I.e. the worker
   * stores the payload
   * @param initPayload The initialization data.
   * @return TRUE if initialization was successfull. FALSE otherwise.
   */
  virtual bool initialize(std::shared_ptr<InitPayload> initPayload) override;

  /**
   * @brief Triggers a configuration of the network worker.
   * @param configPayload The initialization data.
   * @return TRUE if initialization was successfull. FALSE otherwise.
   */
  virtual bool
  configure(std::shared_ptr<ConfigurationPayload> configPayload) override;

  virtual bool write(std::shared_ptr<WriteDeviceMessage> writeMsg) override;

  virtual bool write(std::shared_ptr<InitDeviceMessage> initMsg) override;

  virtual bool write(std::shared_ptr<ConfigDeviceMessage> configMsg) override;

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
   * @brief Worker function that is used by the std::listener thread.
   */
  void listenWorker();

  /**
   * @brief Worker function that is used by the comm thread.
   */
  void commWorker();

  /**
   * @brief Returns the name of the worker.
   * @return A string that identifies the type of the worker.
   */
  virtual std::string getWorkerName() override;

private:
  /**
   * @brief Handles the state machine, in case the socket got closed. If the
   * worker is configured as server, the comm thread is stopped, and the
   * listener thread is started again. If the worker is configured as client, it
   * goes into error state.
   */
  void handleLostConnection();

  std::shared_ptr<NetworkWorkerInitPayload> initPayload;

  std::shared_ptr<SocketWrapper> socketWrapper;

  std::unique_ptr<std::thread> commThread;

  std::unique_ptr<std::thread> listenerThread;

  NetworkWorkerCommState commState;

  bool doComm;

  /// The read buffer used by the comm thread.
  std::vector<unsigned char> readBuffer;

  /// Buffer for the messages that shall be sent over the network.
  std::queue<std::shared_ptr<DeviceMessage>> outgoingNetworkMessages;

  /// Mutex that guards the buffer for the outgoing messages.
  std::mutex outgoingNetworkMessagesMutex;
};
} // namespace Workers

#endif
