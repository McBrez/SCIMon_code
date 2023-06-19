// 3rd party
#include <easylogging++.h>

// Project includes
#include <common.hpp>
#include <message_distributor.hpp>
#include <message_factory.hpp>
#include <network_worker.hpp>

using namespace Messages;

namespace Workers {
NetworkWorker::NetworkWorker()
    : Worker(), socketWrapper(SocketWrapper::getSocketWrapper()),
      doListen(new bool(false)), doComm(false),
      commState(NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_INVALID),
      commThread(nullptr), listenerThread(nullptr) {}

NetworkWorker::~NetworkWorker() {
  *this->doListen = false;
  this->doComm = false;

  if (this->commThread) {
    this->commThread->join();
  }
  if (this->listenerThread) {
    this->listenerThread->join();
  }
}

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
    // Start the std::listener thread.
    this->workerState = DeviceStatus::BUSY;
    this->commState =
        NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_LISTENING;
    *this->doListen = true;
    this->listenerThread.reset(
        new std::thread(&NetworkWorker::listenWorker, this));

  } else if (NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_CLIENT ==
             this->initPayload->getOperationMode()) {
    // Check if the pointer to the comm thread is not null.
    if (commThread) {
      // A thread seems to have existed before. Be sure to join the thread.
      this->doComm = false;
      commThread->join();
      this->commThread.release();
    }

    // Start comm thread.
    this->workerState = DeviceStatus::BUSY;
    this->commState = NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_STARTING;
    this->doComm = true;
    this->commThread.reset(new std::thread(&NetworkWorker::commWorker, this));

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

bool NetworkWorker::initialize(std::shared_ptr<InitPayload> initPayload) {

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

  this->readBuffer.clear();
  this->workerState = DeviceStatus::INITIALIZED;
  this->initPayload = castedInitPayload;
  return true;
}

bool NetworkWorker::configure(
    std::shared_ptr<ConfigurationPayload> configPayload) {
  // Nothing to do. Just return true.

  return true;
}

bool NetworkWorker::specificWrite(
    std::shared_ptr<WriteDeviceMessage> writeMsg) {
  return false;
}

std::list<std::shared_ptr<DeviceMessage>>
NetworkWorker::specificRead(TimePoint timestamp) {
  return std::list<std::shared_ptr<DeviceMessage>>();
}

bool NetworkWorker::handleResponse(
    std::shared_ptr<ReadDeviceMessage> response) {

  if (NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_INVALID ==
      this->commState) {
    // Invalid state. Do nothing.
    LOG(WARNING) << "Network Worker got a response while in invalid state. It "
                    "will not be handled.";
    return false;
  }

  else if (NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_LISTENING ==
           this->commState) {
    // Nothing to do while std::listening for connections.
    LOG(INFO) << "Network Worker got a response while std::listening for "
                 "connections. It "
                 "will not be handled.";
    return true;
  }

  else if (NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_STARTING ==
           this->commState) {
    // Nothing to do while starting connection.
    LOG(INFO) << "Network Worker got a response while starting up. It "
                 "will not be handled.";
    return true;
  }

  else if (NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_HANDSHAKING ==
           this->commState) {
    // Nothing to do while starting connection.
    LOG(INFO) << "Network Worker got a response while initiating the "
                 "handshake. It will not be handled.";
    return true;
  }

  else if (NetworkWorkerCommState::
               NETWORK_WOKER_COMM_STATE_WAITING_FOR_HANDSHAKE_RESPONSE ==
           this->commState) {
    // Nothing to do while waiting for handshake response.
    LOG(INFO) << "Network Worker got a response while waiting for the "
                 "handshake response. It will not be handled.";
    return true;
  }

  else if (NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_WORKING ==
           this->commState) {
    // Check if the received response belongs to a device this object is a proxy
    // of.
    std::list<UserId> proxyIds = this->getProxyUserIds();
    auto it =
        find(proxyIds.begin(), proxyIds.end(), response->getDestination());
    if (it != proxyIds.end()) {
      // This object is a proxy of the destination. push the message to the
      // outgoing network buffer.
      this->outgoingNetworkMessagesMutex.lock();
      this->outgoingNetworkMessages.push(response);
      this->outgoingNetworkMessagesMutex.unlock();

      return true;
    } else {
      // This is an unsolicited response.
      LOG(WARNING) << "Network worker received an unsolicited response. The "
                      "message will be ignored.";

      return false;
    }

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
    LOG(INFO) << "Network Worker starts std::listening on port "
              << this->initPayload->getPort() << ".";
    this->socketWrapper->close();
    this->socketWrapper->clear();

    bool success = this->socketWrapper->listenConnection(
        this->doListen, this->initPayload->getPort());
    LOG(INFO) << "Network Worker finished std::listening on port "
              << this->initPayload->getPort() << ".";

    if (success) {
      // Create comm worker thread.
      LOG(INFO)
          << "Network Worker established connection. Starting comm thread.";
      this->commState =
          NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_HANDSHAKING;
      this->doComm = true;
      this->commThread.reset(new std::thread(&NetworkWorker::commWorker, this));
    }
  } else {
    LOG(ERROR) << "Invalid communication state.";
  }

  LOG(INFO) << "Listener Thread shutting down.";
}

void NetworkWorker::commWorker() {

  LOG(INFO) << "Comm thread of Network Worker started.";

  while (this->doComm) {
    if (NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_STARTING ==
        this->commState) {

      // Client initiates the connection here.
      if (NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_CLIENT ==
          this->initPayload->getOperationMode()) {
        LOG(INFO) << "Network worker starts connection to "
                  << this->initPayload->getIpAddress() << ":"
                  << this->initPayload->getPort();
        bool success = this->socketWrapper->open(
            this->initPayload->getIpAddress(), this->initPayload->getPort());

        if (success) {
          LOG(INFO) << "Network worker successfully connected to "
                    << this->initPayload->getIpAddress() << ":"
                    << this->initPayload->getPort();
          this->commState =
              NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_HANDSHAKING;
        } else {
          LOG(ERROR) << "Network worker was not able to connect to "
                     << this->initPayload->getIpAddress() << ":"
                     << this->initPayload->getPort();
          this->commState =
              NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_ERROR;
        }
      } else {
        // Other roles shouldnt get here.
        this->commState =
            NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_ERROR;
      }
    }

    else if (NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_HANDSHAKING ==
             this->commState) {

      // Server and client have different roles here. Server waits for the
      // handshake message, while the client sends it.
      if (NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_CLIENT ==
          this->initPayload->getOperationMode()) {
        // The client initiates the handshake. Send a handshake message.
        std::shared_ptr<DeviceMessage> handshakeMsg(
            new HandshakeMessage(this->self->getUserId(), UserId(),
                                 this->messageDistributor->getStatus(),
                                 MessageFactory::getInstace()->getVersion()));
        this->socketWrapper->write(
            MessageFactory::getInstace()->encodeMessage(handshakeMsg));
        this->commState =
            NETWORK_WOKER_COMM_STATE_WAITING_FOR_HANDSHAKE_RESPONSE;

      }

      else if (NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_SERVER ==
               this->initPayload->getOperationMode()) {
        // The server just Waits for the hand shake message.
        // This state is only relevant if the worker is configured as
        // client. Read from the socket and try to decode a message from
        // that.
        int readRet = this->socketWrapper->read(this->readBuffer);
        std::shared_ptr<DeviceMessage> msg =
            MessageFactory::getInstace()->decodeMessage(this->readBuffer);

        // Has a message been decoded?
        if (!msg) {
          // No message has been decoded. Continue loop.
          continue;
        }

        // A message has been decoded. Is it a handshake message?
        auto handshakeMsg = dynamic_pointer_cast<HandshakeMessage>(msg);
        if (!handshakeMsg) {
          // The decoded message is not a handshake message. Ignore it and
          // continue.
          LOG(INFO) << "Network worker expected a handshake message. "
                       "Got another message type instead.";
          continue;
        }

        // The decoded message is a handshake message. Add the remote ids to
        // the proxy ids of this object and advance to working state.
        this->clearProxyIds();
        for (auto statusPayload : handshakeMsg->getPayload()) {
          this->addProxyId(statusPayload->getDeviceId());
        }
        // Send back a handshake message.
        std::shared_ptr<DeviceMessage> handshakeMsgResponse(
            new HandshakeMessage(this->self->getUserId(),
                                 handshakeMsg->getSource(),
                                 this->messageDistributor->getStatus(),
                                 MessageFactory::getInstace()->getVersion()));
        this->socketWrapper->write(
            MessageFactory::getInstace()->encodeMessage(handshakeMsgResponse));

        // Handshake was successful. Proceed to next state.
        this->commState =
            NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_WORKING;
        this->workerState = DeviceStatus::OPERATING;
        this->readBuffer.clear();

      }

      else {
        // Undefined operation mode.
        LOG(ERROR) << "Network Worker comm thread got into unknown "
                      "operation mode.";
        this->commState =
            NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_ERROR;
      }

    }

    else if (NetworkWorkerCommState::
                 NETWORK_WOKER_COMM_STATE_WAITING_FOR_HANDSHAKE_RESPONSE ==
             this->commState) {
      // This state is only relevant if the worker is configured as client.
      // Read from the socket and try to decode a message from that.
      int readRet = this->socketWrapper->read(this->readBuffer);
      std::shared_ptr<DeviceMessage> msg =
          MessageFactory::getInstace()->decodeMessage(this->readBuffer);

      // Has a message been decoded?
      if (!msg) {
        // No message has been decoded. Continue loop.
        continue;
      }

      // A message has been decoded. Is it a handshake message?
      auto handshakeMsg = dynamic_pointer_cast<HandshakeMessage>(msg);
      if (!handshakeMsg) {
        // The decoded message is not a handshake message. Ignore it and
        // continue.
        LOG(INFO) << "Network worker expected a handshake response message. "
                     "Got another message type instead.";
        continue;
      }

      // The decoded message is a handshake message. Add the remote ids to
      // the proxy ids of this object and advance to working state.
      this->clearProxyIds();
      for (auto statusPayload : handshakeMsg->getPayload()) {
        this->addProxyId(statusPayload->getDeviceId());
      }
      this->commState =
          NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_WORKING;
      this->workerState = DeviceStatus::OPERATING;
      this->readBuffer.clear();
    }

    else if (NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_WORKING ==
             this->commState) {

      // Worker is in operation mode. Read from socket and try to interpret
      // messages. Put interpreted messages into messageOut. Then send
      // messages meant for the communication partner over.

      // Read from socket.
      int readSuccess = this->socketWrapper->read(this->readBuffer);
      if (readSuccess <= 0) {
        // Connection seems to be closed. Set the worker in an invalid state.
        // LOG(INFO) << "Other end point seems to have closed the connection. "
        //             "Closing down socket.";
        // this->doComm = false;
      }
      std::shared_ptr<DeviceMessage> msg =
          MessageFactory::getInstace()->decodeMessage(this->readBuffer);
      // If a message could be decoded, push it to the queue.
      if (msg) {
        LOG(DEBUG) << "Network worker decoded a " << msg->serialize()
                   << " from " << msg->getSource().id() << ".";
        this->pushMessageQueue(msg);
      }

      // Write message to socket.
      this->outgoingNetworkMessagesMutex.lock();
      while (!this->outgoingNetworkMessages.empty()) {
        std::shared_ptr<DeviceMessage> message =
            this->outgoingNetworkMessages.front();
        this->outgoingNetworkMessages.pop();
        int sendSuccess = this->socketWrapper->write(
            MessageFactory::getInstace()->encodeMessage(message));
      }
      this->outgoingNetworkMessagesMutex.unlock();
    }

    else {
      LOG(ERROR) << "Invalid communication state. Aborting.";

      break;
    }
  }

  this->commState = NetworkWorkerCommState::NETWORK_WOKER_COMM_STATE_INVALID;
  LOG(INFO) << "Network Worker thread is finished. Shutting down.";
  this->socketWrapper->close();
}

bool NetworkWorker::write(std::shared_ptr<HandshakeMessage> writeMsg) {
  return true;
}

bool NetworkWorker::write(std::shared_ptr<WriteDeviceMessage> writeMsg) {
  // Does the message target this object?
  if (this->isExactTarget(writeMsg->getDestination())) {
    // This object is targeted. Just execute the default method.
    return Worker::write(writeMsg);
  } else {
    // An proxy held by this object is targeted. Put the message into the
    // outgoing network queue.
    this->outgoingNetworkMessagesMutex.lock();
    this->outgoingNetworkMessages.push(writeMsg);
    this->outgoingNetworkMessagesMutex.unlock();

    return true;
  }
}

bool NetworkWorker::write(std::shared_ptr<InitDeviceMessage> initMsg) {
  // Does the message target this object?
  if (this->isExactTarget(initMsg->getDestination())) {
    // This object is targeted. Just execute the default method.
    return Worker::write(initMsg);
  } else {
    // An proxy held by this object is targeted. Put the message into the
    // outgoing network queue.
    this->outgoingNetworkMessagesMutex.lock();
    this->outgoingNetworkMessages.push(initMsg);
    this->outgoingNetworkMessagesMutex.unlock();

    return true;
  }
}

bool NetworkWorker::write(std::shared_ptr<ConfigDeviceMessage> configMsg) {
  // Does the message target this object?
  if (this->isExactTarget(configMsg->getDestination())) {
    // This object is targeted. Just execute the default method.
    return Worker::write(configMsg);
  } else {
    // An proxy held by this object is targeted. Put the message into the
    // outgoing network queue.
    this->outgoingNetworkMessagesMutex.lock();
    this->outgoingNetworkMessages.push(configMsg);
    this->outgoingNetworkMessagesMutex.unlock();

    return true;
  }
}

std::string NetworkWorker::getWorkerName() {
  return Core::NETWORK_WORKER_TYPE_NAME;
}

} // namespace Workers
