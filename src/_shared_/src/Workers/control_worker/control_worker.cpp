// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <control_worker.hpp>
#include <message_distributor.hpp>
#include <network_worker_init_payload.hpp>

using namespace Workers;

const std::string ControlWorker::UnknownDeviceSpecifier = "UNKNOWN DEVICE";

ControlWorker::ControlWorker()
    : controlWorkerSubState(
          ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_IVALID),
      doQueryState(false) {}

ControlWorker::~ControlWorker() {}

void ControlWorker::work(TimePoint timestamp) {}

bool ControlWorker::start() {
  // Check if control worker is in correct state.
  if (this->workerState != DeviceStatus::IDLE ||
      this->controlWorkerSubState !=
          ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_REMOTE_STOPPED) {
  }

  return false;
}

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
        return false;
      }
      // This should be a status response payload. Cast it down and check it.
      auto statusPayload =
          dynamic_pointer_cast<StatusPayload>(response->getReadPaylod());
      if (!statusPayload) {
        LOG(WARNING) << "Control worker received unexpected payload.";
        return false;
      }
      // Has it already connected?
      if (DeviceStatus::OPERATING == statusPayload->getDeviceStatus()) {
        // Connection established. Set new state and remember the proxy ids.
        for (auto proxyId : statusPayload->getProxyIds()) {
          this->remoteHostIds[proxyId.id()] =
              std::make_tuple(false, ControlWorker::UnknownDeviceSpecifier,
                              DeviceType::INVALID);
        }
        this->controlWorkerSubState =
            ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_CONNECTED;

        return true;
      } else {
        // Connection has not been established yet. Has the time out elapsed?
        if ((std::chrono::system_clock::now() - this->connectionTimeout) >
            std::chrono::seconds(10)) {
          // Time out has elapsed. Revert to previous state.
          this->controlWorkerSubState = ControlWorkerSubState::
              CONTROL_WORKER_SUBSTATE_WAITING_FOR_CONNECTION;
        } else {
          // Time out has not yet elapsed. Send another query state message.
          this->pushMessageQueue(
              std::shared_ptr<DeviceMessage>(new WriteDeviceMessage(
                  this->getUserId(), response->getSource(),
                  WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));
        }
        return true;
      }

    }

    else if (ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_CONF_EXPLORE ==
             this->controlWorkerSubState) {
      // Is the response from one of the remote ids?
      if (!this->remoteHostIds.contains(response->getSource().id())) {
        LOG(WARNING)
            << "Control worker received response from unexpected source";
        return false;
      }
      // This should be a status response payload. Cast it down and check it.
      auto statusPayload =
          dynamic_pointer_cast<StatusPayload>(response->getReadPaylod());
      if (!statusPayload) {
        LOG(WARNING) << "Control worker received unexpected payload.";
        return false;
      }

      // Remember the proxy IDs.
      this->remoteHostIds[statusPayload->getDeviceId().id()] = std::make_tuple(
          true, statusPayload->getDeviceName(), statusPayload->getDeviceType());

      // Check if all remote ids are now resolved.
      bool everythingResolved = true;
      for (auto key : this->remoteHostIds) {
        if (std::get<0>(key.second) == false) {
          everythingResolved = false;
          break;
        }
      }
      if (everythingResolved) {
        // All remote ids are resolved. Adjust state and send init and config
        // message.
        this->controlWorkerSubState =
            ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_CONF_REMOTE;
        this->pushMessageQueue(std::shared_ptr<DeviceMessage>(
            new InitDeviceMessage(this->getUserId(), this->getSentryId(),
                                  this->remoteInitPayload)));
        this->pushMessageQueue(std::shared_ptr<DeviceMessage>(
            new ConfigDeviceMessage(this->getUserId(), this->getSentryId(),
                                    this->remoteConfigPayload)));

        // Start the query remote state worker.
        this->doQueryState = false;
        if (this->queryRemoteStateThread) {
          this->queryRemoteStateThread->join();
        }
        this->doQueryState = true;
        this->queryRemoteStateThread.reset(
            new std::thread(&ControlWorker::queryRemoteStateWorker, this));
      }

      return true;
    }

    else if (ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_CONF_REMOTE ==
             this->controlWorkerSubState) {
      this->handleStatusPayload(response);
    }

    else {
      LOG(WARNING)
          << "Control worker does not expect responses in the current state.";
      return false;
    }
  }

  else {
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
  this->pushMessageQueue(std::shared_ptr<DeviceMessage>(new InitDeviceMessage(
      this->getUserId(), this->networkWorkerId,
      new NetworkWorkerInitPayload(
          NetworkWorkerOperationMode::NETWORK_WORKER_OP_MODE_CLIENT, ip,
          port))));
  this->pushMessageQueue(std::shared_ptr<DeviceMessage>(
      new WriteDeviceMessage(this->getUserId(), this->networkWorkerId,
                             WriteDeviceTopic::WRITE_TOPIC_RUN)));
  // Begin querying the state of the network worker, in order to check if the
  // connection is successfull.
  this->connectionTimeout = std::chrono::system_clock::now();
  this->pushMessageQueue(std::shared_ptr<DeviceMessage>(
      new WriteDeviceMessage(this->getUserId(), this->networkWorkerId,
                             WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));

  return true;
}

bool ControlWorker::startConfig(
    std::shared_ptr<SentryInitPayload> initPayload,
    std::shared_ptr<SentryConfigPayload> configPayload) {
  // Is worker in correct state?
  if (this->workerState != DeviceStatus::IDLE) {
    LOG(WARNING)
        << "Control Worker received a request to configure the host, but "
           "it is not in the correct state.";
    return false;
  }
  if (this->controlWorkerSubState !=
      ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_CONNECTED) {
    LOG(WARNING)
        << "Control Worker received a request to configure the host, but "
           "it is not in the correct state.";
    return false;
  }

  // Cache the init and config payload for later.
  this->remoteInitPayload = initPayload;
  this->remoteConfigPayload = configPayload;

  // Adjust sub state and send a status message to each of the proxy ids..
  this->controlWorkerSubState =
      ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_CONF_EXPLORE;
  for (auto keyValuePair : this->remoteHostIds) {
    this->pushMessageQueue(std::shared_ptr<DeviceMessage>(
        new WriteDeviceMessage(this->getUserId(), keyValuePair.first,
                               WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));
  }

  return true;
}

ControlWorkerSubState ControlWorker::getControlWorkerState() const {
  return this->controlWorkerSubState;
}

UserId ControlWorker::getSentryId() {
  UserId retVal;
  for (auto keyValuePair : this->remoteHostIds) {
    if (get<1>(keyValuePair.second) == SENTRY_WORKER_TYPE_NAME) {
      retVal = keyValuePair.first;
      break;
    }
  }

  return retVal;
}

void ControlWorker::queryRemoteStateWorker() {
  while (this->doQueryState) {
    // Check if worker is still in the correct state.
    if (this->controlWorkerSubState == CONTROL_WORKER_SUBSTATE_CONF_REMOTE ||
        this->controlWorkerSubState == CONTROL_WORKER_SUBSTATE_REMOTE_RUNNING ||
        this->controlWorkerSubState == CONTROL_WORKER_SUBSTATE_REMOTE_STOPPED) {

      // Send a query state message to each remote id.
      for (auto keyValuePair : this->remoteHostIds) {
        this->pushMessageQueue(std::shared_ptr<DeviceMessage>(
            new WriteDeviceMessage(this->getUserId(), keyValuePair.first,
                                   WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));
      }

    } else {
      break;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

std::list<std::shared_ptr<StatusPayload>> ControlWorker::getRemoteStatus() {
  return this->remoteStatus;
}

bool ControlWorker::handleStatusPayload(
    std::shared_ptr<ReadDeviceMessage> msg) {

  // Try to cast down the payload.
  auto statusPayload =
      dynamic_pointer_cast<StatusPayload>(msg->getReadPaylod());
  if (!statusPayload) {
    // This is not a status message. Return here.
    return false;
  }

  // If device, the status payload originates from, is already known, replace
  // the entry, otherwise add an entry.
  auto it = std::find_if(
      this->remoteStatus.begin(), this->remoteStatus.end(),
      [statusPayload](std::shared_ptr<StatusPayload> currRemoteStatus) {
        return currRemoteStatus->getDeviceId() == statusPayload->getDeviceId();
      });

  if (it == this->remoteStatus.end()) {
    this->remoteStatus.push_back(statusPayload);
  } else {
    *it = statusPayload;
  }

  return true;
}
