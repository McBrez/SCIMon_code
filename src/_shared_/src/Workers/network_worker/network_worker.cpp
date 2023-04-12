// 3rd party
#include <easylogging++.h>

// Project includes
#include <message_distributor.hpp>
#include <network_worker.hpp>

using namespace Messages;

namespace Workers {
NetworkWorker::NetworkWorker()
    : Worker(), socketWrapper(SocketWrapper::getSocketWrapper()),
      doListen(new bool(false)), doComm(false),
      commState(NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_INVALID) {}

NetworkWorker::~NetworkWorker() {}

void NetworkWorker::work(TimePoint timestamp) {}

bool NetworkWorker::start() {
  if (DeviceStatus::INITIALIZED != this->getState() &&
      DeviceStatus::IDLE != this->getState()) {
    LOG(WARNING)
        << "Can not start Network Worker as it is not in the correct state.";
    return false;
  }

  if (NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_SERVER ==
      this->initPayload->getOperationMode()) {
    // Start the listener thread.
    this->workerState = DeviceStatus::BUSY;
    this->commState =
        NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_LISTENING;
    this->listenerThread.reset(new thread(&NetworkWorker::listenWorker, this));

  } else if (NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_CLIENT ==
             this->initPayload->getOperationMode()) {
    // Start comm thread.
    this->workerState = DeviceStatus::BUSY;
    this->commState =
        NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_HANDSHAKING;
    this->doComm = true;
    this->commThread.reset(new thread(&NetworkWorker::commWorker, this));

  } else {
    this->workerState = DeviceStatus::ERROR;
    LOG(WARNING) << "Unknown operation mode requested from Network Worker.";
    return false;
  }

  return true;
}

bool NetworkWorker::stop() {
  if (DeviceStatus::OPERATING != this->getState() &&
      DeviceStatus::BUSY != this->getState()) {
    LOG(WARNING)
        << "Can not stop Network Worker, as it is currently not connected.";
  }

  this->workerState = DeviceStatus::IDLE;
  this->doComm = false;
  *this->doListen = false;

  this->commState = NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_INVALID;

  return true;
}

bool NetworkWorker::initialize(shared_ptr<InitPayload> initPayload) {

  if (!initPayload) {
    LOG(ERROR) << "Received malformed init message.";
    return false;
  }
  auto castedInitPayload =
      dynamic_pointer_cast<NetworkWorkerInitPayload>(initPayload);
  if (!castedInitPayload) {
    LOG(ERROR) << "Received malformed init message.";
    return false;
  }

  this->workerState = DeviceStatus::INITIALIZED;
  this->initPayload = castedInitPayload;
  return true;
}

bool NetworkWorker::configure(shared_ptr<ConfigurationPayload> configPayload) {
  // Nothing to do. Just return true.

  return true;
}

bool NetworkWorker::specificWrite(shared_ptr<WriteDeviceMessage> writeMsg) {
  return false;
}

list<shared_ptr<DeviceMessage>>
NetworkWorker::specificRead(TimePoint timestamp) {
  return list<shared_ptr<DeviceMessage>>();
}

bool NetworkWorker::handleResponse(shared_ptr<ReadDeviceMessage> response) {

  if (NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_INVALID ==
      this->commState) {
    // Invalid state. Do nothing.
    return false;
  }

  else if (NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_LISTENING ==
           this->commState) {
    // Nothing to do while listening for connections.
    return true;
  }

  else if (NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_STARTING ==
           this->commState) {
    // Nothing to do while starting connection.
    return true;
  }

  else if (NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_HANDSHAKING ==
           this->commState) {
    // Nothing to do while starting connection.
    return true;
  }

  else if (NetworkWorkerCommState::
               NETWORK_WOKER_COMM_STATE_WAITING_FOR_HANDSHAKE_RESPONSE ==
           this->commState) {
    // Nothing to do while starting connection.
    return true;
  }

  else if (NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_WORKING ==
           this->commState) {
    // Nothing to do while starting connection.
    return true;
  }

  else if (NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_ERROR ==
           this->commState) {
    // Nothing to do while in error state.
    return false;
  }

  else {
    // Unknown state. Return to error.
    LOG(ERROR) << "Network Worker is in unknown state.";
    this->commState = NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_ERROR;
    return false;
  }
}

void NetworkWorker::listenWorker() {

  if (NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_LISTENING ==
      this->commState) {
    LOG(INFO) << "Network Worker starts listening on port "
              << this->initPayload->getPort() << ".";
    this->socketWrapper->close();
    this->socketWrapper->clear();

    bool success = this->socketWrapper->listenConnection(
        this->doListen, this->initPayload->getPort());
    LOG(INFO) << "Network Worker finished listening on port "
              << this->initPayload->getPort() << ".";

    if (success) {
      // Create comm worker thread.
      LOG(INFO)
          << "Network Worker established connection. Starting comm thread.";
      this->commState =
          NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_HANDSHAKING;
      this->doComm = true;
      this->commThread.reset(new thread(&NetworkWorker::commWorker, this));
    }
  } else {
    LOG(ERROR) << "Invalid communication state.";
  }

  LOG(INFO) << "Listener Thread shutting down.";
}

void NetworkWorker::commWorker() {

  LOG(INFO) << "Comm thread of Network Worker started.";

  while (this->doComm) {
    if (NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_HANDSHAKING ==
        this->commState) {

      // Server and client have different roles here. Server waits for the
      // handshake message, while the client sends it.
      if (NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_CLIENT ==
          this->initPayload->getOperationMode()) {
        // The client initiates the handshake. Send a query state message.
        shared_ptr<DeviceMessage> handshakeMsg(
            new HandshakeMessage(this->self->getUserId(), UserId(),
                                 this->messageDistributor->getStatus()));
        this->socketWrapper->write(handshakeMsg->bytes());
        this->commState =
            NETWORK_WOKER_COMM_STATE_WAITING_FOR_HANDSHAKE_RESPONSE;

      }

      else if (NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_SERVER ==
               this->initPayload->getOperationMode()) {
        // The server just Waits for the hand shake message.
      }

      else {
        // Undefined operation mode.
        LOG(ERROR)
            << "Network Worker comm thread got into unknown operation mode.";
        this->commState =
            NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_ERROR;
      }

    }

    else if (NetworkWorkerCommState::
                 NETWORK_WOKER_COMM_STATE_WAITING_FOR_HANDSHAKE_RESPONSE ==
             this->commState) {
      // This state is only relevant if the worker is configured as client.
      // Check if the handshake response already arrived.
    }

    else if (NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_WORKING ==
             this->commState) {
    }

    else {
      LOG(ERROR) << "Invalid communication state. Aborting.";
      break;
    }
  }

  LOG(INFO) << "Network Worker thread is finished. Shutting down.";
}

bool NetworkWorker::write(shared_ptr<HandshakeMessage> writeMsg) {
  return true;
}

} // namespace Workers