#ifndef NETWORK_WORKER_HPP
#define NETWORK_WORKER_HPP

// Project includes
#include <network_worker_init_payload.hpp>
#include <socket_wrapper.hpp>
#include <worker.hpp>

using namespace Utilities;

namespace Workers {

/// Identifies the states of the network communication.
enum NetworkWorkerCommState {
  /// Invalid state.
  NETWORK_WOKER_COMM_STATE_INVALID,
  /// Worker is listening to connection requests.
  NETWORK_WOKER_COMM_STATE_LISTENING,
  /// Worker is starting up.
  NETWORK_WOKER_COMM_STATE_STARTING,
  /// Worker is sending the handshake message.
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
  NetworkWorker();

  virtual ~NetworkWorker() override;

  virtual void work(TimePoint timestamp) override;

  virtual bool start();

  virtual bool stop();

  virtual bool initialize(shared_ptr<InitPayload> initPayload) override;

  virtual bool
  configure(shared_ptr<ConfigurationPayload> configPayload) override;

  /**
   * @brief Handles a device specific message. Called by
   * write(shared_ptr<WriteDeviceMessage>), if the mssage could not be resolved.
   * @param writeMsg The device specific message.
   * @return True if succesful. False otherwise.
   */
  virtual bool specificWrite(shared_ptr<WriteDeviceMessage> writeMsg) override;

  /**
   * @brief Device-specific read operation, that is executed on each call of
   * read(). Appends to the outgoing message queue.
   * @param timestamp The time at which this method is called.
   * @return A read message.
   */
  virtual list<shared_ptr<DeviceMessage>>
  specificRead(TimePoint timestamp) override;

  /**
   * @brief Handles the response to a write message that has been sent by this
   * object.
   * @param response The response to a write message that has been sent earlier.
   * @return TRUE if the response has been handled successfully. False
   * otherwise.
   */
  virtual bool handleResponse(shared_ptr<ReadDeviceMessage> response) override;

  void listenWorker();

  void commWorker();

private:
  shared_ptr<NetworkWorkerInitPayload> initPayload;

  shared_ptr<SocketWrapper> socketWrapper;

  unique_ptr<thread> commThread;

  unique_ptr<thread> listenerThread;

  shared_ptr<bool> doListen;

  NetworkWorkerCommState commState;

  bool doComm;
};
} // namespace Workers

#endif