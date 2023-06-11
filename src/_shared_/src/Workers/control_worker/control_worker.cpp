// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <control_worker.hpp>
#include <message_distributor.hpp>
#include <network_worker_init_payload.hpp>

using namespace Workers;

ControlWorker::ControlWorker()
    : controlWorkerSubState(
          ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_IVALID) {}

ControlWorker::~ControlWorker() {}

void ControlWorker::work(TimePoint timestamp) {}

bool ControlWorker::start() { return false; }

bool ControlWorker::stop() { return false; }

bool ControlWorker::initialize(std::shared_ptr<InitPayload> initPayload) {
  LOG(INFO) << "Control Worker is initializing";
  this->workerState = DeviceStatus::INITIALIZING;

  // Try to find the network worker.
  std::list<std::shared_ptr<StatusPayload>> statusPayloads =
      this->messageDistributor->getStatus();
  bool foundNetworkWorker = false;
  for (auto statusPayload : statusPayloads) {
    if (statusPayload->getDeviceName() == NETWORK_WORKER_TYPE_NAME) {
      this->networkWorkerId = statusPayload->getDeviceId();
      foundNetworkWorker = true;
      break;
    }
  }
  if (!foundNetworkWorker) {
    LOG(ERROR) << "Control Worker did not find the network worker. This "
                  "configuration will no work";
    this->workerState = DeviceStatus::ERROR;
    return false;
  }

  LOG(INFO) << "Control Worker found the network worker.";
  this->workerState = DeviceStatus::IDLE;

  return true;
}

bool ControlWorker::configure(
    std::shared_ptr<ConfigurationPayload> configPayload) {
  // Check if control worker is in the correct state.
  if (DeviceStatus::INITIALIZED != this->workerState) {
    LOG(WARNING) << "Control worker received a configure request, but "
                    "currently is not in the correct state.";
    return false;
  }

  LOG(INFO) << "Control Worker is configuring";
  this->workerState = DeviceStatus::CONFIGURING;
  this->controlWorkerSubState = CONTROL_WORKER_SUBSTATE_WAITING_FOR_CONNECTION;

  return false;
}

bool ControlWorker::write(std::shared_ptr<HandshakeMessage> writeMsg) {
  return false;
}

bool ControlWorker::specificWrite(
    std::shared_ptr<WriteDeviceMessage> writeMsg) {
  return false;
}

std::list<std::shared_ptr<DeviceMessage>>
ControlWorker::specificRead(TimePoint timestamp) {
  return std::list<std::shared_ptr<DeviceMessage>>();
}

bool ControlWorker::handleResponse(
    std::shared_ptr<ReadDeviceMessage> response) {

  if (DeviceStatus::IDLE == this->workerState) {
    if (CONTROL_WORKER_SUBSTATE_CONNECTING == this->controlWorkerSubState) {
      // Is the response from the network worker?
      if (response->getSource() != this->networkWorkerId) {
        LOG(WARNING)
            << "Control worker received response from unexpected source";
      }
      // This should be a status response payload. Cast it down and check it.
      auto statusPayload =
          dynamic_pointer_cast<StatusPayload>(response->getReadPaylod());
      if (!statusPayload) {
        LOG(WARNING) << "Control worker received unexpected payload.";
      }
      // Has it already connected?
      if (DeviceStatus::OPERATING == statusPayload->getDeviceStatus()) {
        // Connection established. Set new state.
        this->controlWorkerSubState =
            ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_CONNECTED;

        return true;
      } else {
        // It is not. Send another status request message.
        this->messageOut.push(std::shared_ptr<DeviceMessage>(
            new WriteDeviceMessage(this->getUserId(), response->getSource(),
                                   WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));

        return true;
      }

    } else {
      LOG(WARNING)
          << "Control worker does not expect responses in the current state.";
      return false;
    }
  } else {
    LOG(WARNING)
        << "Control worker does not expect responses in the current state.";
    return false;
  }
}

std::string ControlWorker::getWorkerName() { return ""; }

bool ControlWorker::startConnect(std::string ip, int port) {
  if (this->workerState != DeviceStatus::IDLE) {
    LOG(WARNING) << "Control Worker received a request to connect to host, but "
                    "it is not in the correct state.";
    return false;
  }

  LOG(INFO) << "Control worker is initiating connection to " << ip << ":"
            << port;
  this->controlWorkerSubState = CONTROL_WORKER_SUBSTATE_CONNECTING;
  // Send the init and start message to the network worker.
  this->messageOut.push(std::shared_ptr<DeviceMessage>(new InitDeviceMessage(
      this->getUserId(), this->networkWorkerId,
      new NetworkWorkerInitPayload(
          NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_CLIENT, ip,
          port))));
  this->messageOut.push(std::shared_ptr<DeviceMessage>(
      new WriteDeviceMessage(this->getUserId(), this->networkWorkerId,
                             WriteDeviceTopic::WRITE_TOPIC_RUN)));
  // Begin querying the state of the network worker, in order to check if the
  // connection is successfull.
  this->messageOut.push(std::shared_ptr<DeviceMessage>(
      new WriteDeviceMessage(this->getUserId(), this->networkWorkerId,
                             WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));

  return true;
}

bool ControlWorker::startConfig() { return true; }

ControlWorkerSubState ControlWorker::getControlWorkerState() const {
  return this->controlWorkerSubState;
}
