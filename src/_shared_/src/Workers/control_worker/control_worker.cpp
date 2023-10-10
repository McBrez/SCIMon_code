// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <common.hpp>
#include <control_worker.hpp>
#include <data_response_payload.hpp>
#include <key_response_payload.hpp>
#include <message_distributor.hpp>
#include <network_worker_init_payload.hpp>
#include <request_data_payload.hpp>

using namespace Workers;

const std::string ControlWorker::UnknownDeviceSpecifier = "UNKNOWN DEVICE";

ControlWorker::ControlWorker()
    : controlWorkerSubState(
          ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_IVALID),
      doQueryState(false), dataQueryInterval(std::chrono::seconds(5)),
      doQueryData(false) {}

ControlWorker::~ControlWorker() {}

void ControlWorker::work(TimePoint timestamp) {}

bool ControlWorker::start() {
  // Check if control worker is in correct state.
  if (this->workerState == DeviceStatus::IDLE &&
      this->controlWorkerSubState ==
          ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_CONF_REMOTE) {

    // Send the start message to the remote end and adjust status.
    this->controlWorkerSubState =
        ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_REMOTE_RUNNING;
    this->workerState = DeviceStatus::OPERATING;
    this->pushMessageQueue(std::shared_ptr<DeviceMessage>(
        new WriteDeviceMessage(this->getUserId(), this->getSentryId(),
                               WriteDeviceTopic::WRITE_TOPIC_RUN)));

    return true;
  }

  return false;
}

bool ControlWorker::stop() {
  // Check if control worker is in correct state.
  if (this->workerState == DeviceStatus::OPERATING) {
    // Send the stop message to the remote end and adjust status.
    this->controlWorkerSubState =
        ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_REMOTE_STOPPED;
    this->workerState = DeviceStatus::IDLE;
    this->pushMessageQueue(std::shared_ptr<DeviceMessage>(
        new WriteDeviceMessage(this->getUserId(), this->getSentryId(),
                               WriteDeviceTopic::WRITE_TOPIC_STOP)));

    return true;
  }

  return false;
}

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

  this->onInitialized(this->getWorkerName(), KeyMapping(), SpectrumMapping());

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

  LOG(INFO) << "Control Worker has been configured";
  this->workerState = DeviceStatus::IDLE;
  this->controlWorkerSubState = CONTROL_WORKER_SUBSTATE_WAITING_FOR_CONNECTION;

  this->onConfigured(KeyMapping(), SpectrumMapping());

  return true;
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
    // Handle the status of the remote end.
    this->handleStatusPayload(response);

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
        for (auto &proxyId : statusPayload->getProxyIds()) {
          this->remoteHostIds[proxyId.id()] =
              std::make_tuple(false, ControlWorker::UnknownDeviceSpecifier,
                              DeviceType::INVALID);
        }
        this->controlWorkerSubState =
            ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_CONNECTED;

        return true;
      } else {
        // Connection has not been established yet. Has the time out elapsed?
        if ((Core::getNow() - this->connectionTimeout) >
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
            ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_INIT_REMOTE;
        this->pushMessageQueue(std::shared_ptr<DeviceMessage>(
            new InitDeviceMessage(this->getUserId(), this->getSentryId(),
                                  this->remoteInitPayload)));

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

    else if (ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_INIT_REMOTE ==
             this->controlWorkerSubState) {
      // Wait until the remote end has initialized. Then send the config
      // message.
      // Is the response from the sentry?
      if (response->getSource() == this->getSentryId()) {
        auto statusPayload =
            dynamic_pointer_cast<StatusPayload>(response->getReadPaylod());
        if (!statusPayload) {
          return true;
        }

        if (statusPayload->getDeviceStatus() == DeviceStatus::INITIALIZED) {
          this->pushMessageQueue(std::shared_ptr<DeviceMessage>(
              new ConfigDeviceMessage(this->getUserId(), this->getSentryId(),
                                      this->remoteConfigPayload)));
          this->controlWorkerSubState =
              ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_CONF_REMOTE;

          return true;
        }
      }
    }

    else if (ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_CONF_REMOTE ==
             this->controlWorkerSubState) {
      // Handle the status response.
      this->handleStatusPayload(response);

      // If all remote devices are in IDLE (i.e. their configuration succeeded),
      // send a message to request their data keys.
      auto pumpIt = std::find_if(
          this->remoteStatus.begin(), this->remoteStatus.end(),
          [](std::shared_ptr<StatusPayload> status) {
            return status->getDeviceStatus() == DeviceStatus::IDLE &&
                   status->getDeviceType() == DeviceType::PUMP_CONTROLLER;
          });
      auto spectrometerIt = std::find_if(
          this->remoteStatus.begin(), this->remoteStatus.end(),
          [](std::shared_ptr<StatusPayload> status) {
            return status->getDeviceStatus() == DeviceStatus::IDLE &&
                   status->getDeviceType() ==
                       DeviceType::IMPEDANCE_SPECTROMETER;
          });

      if (pumpIt != this->remoteStatus.end() &&
          spectrometerIt != this->remoteStatus.end()) {
        // Configuration succeeded for the remote devices. Request their data
        // keys by sending corresponding messages.

        std::shared_ptr<DeviceMessage> requestPumpKeyMsg(
            new WriteDeviceMessage(this->getUserId(), (*pumpIt)->getDeviceId(),
                                   WriteDeviceTopic::WRITE_TOPIC_REQUEST_KEYS));
        std::shared_ptr<DeviceMessage> requestSpectrometerKeyMsg(
            new WriteDeviceMessage(this->getUserId(),
                                   (*spectrometerIt)->getDeviceId(),
                                   WriteDeviceTopic::WRITE_TOPIC_REQUEST_KEYS));

        this->controlWorkerSubState =
            CONTROL_WORKER_SUBSTATE_CONF_GET_DATA_KEYS;
        this->pushMessageQueue(requestPumpKeyMsg);
        this->pushMessageQueue(requestSpectrometerKeyMsg);

      } else {
        // Remote devices are not yet ready. Do nothing.
      }

      return true;

    } else if (ControlWorkerSubState::
                   CONTROL_WORKER_SUBSTATE_CONF_GET_DATA_KEYS ==
               this->controlWorkerSubState) {
      // Handle the status response.
      this->handleStatusPayload(response);

      // Downcast and check if this is a KeyResponsePayload.
      auto keyResponsePayload =
          dynamic_pointer_cast<KeyResponsePayload>(response->getReadPaylod());
      if (!keyResponsePayload) {
        return true;
      }

      // Save the data keys.
      this->remoteDataKeys[response->getSource().id()] =
          std::make_tuple(keyResponsePayload->getKeyMapping(),
                          keyResponsePayload->getSpectrumMapping());

      // Check if the data keys of all remote devices are now known.
      if (this->remoteDataKeys.contains(this->getPumpControllerId().id()) &&
          this->remoteDataKeys.contains(this->getSpectrometerId().id())) {

        // Create those keys in the local data manager.
        for (auto &remoteDataKeysEntry : this->remoteDataKeys) {
          KeyMapping keyMapping =
              std::get<KeyMapping>(remoteDataKeysEntry.second);
          for (auto &keys : keyMapping) {
            std::string keyName =
                std::to_string(remoteDataKeysEntry.first) + "/" + keys.first;
            this->dataManager->createKey(keyName, keys.second);
            // If the key corresponds to a spectrum, it has to be set up.
            if (keys.second == Utilities::DATAMANAGER_DATA_TYPE_SPECTRUM) {
              SpectrumMapping spectrumMapping =
                  std::get<SpectrumMapping>(remoteDataKeysEntry.second);
              this->dataManager->setupSpectrum(keyName,
                                               spectrumMapping[keys.first]);
            }
          }
        }

        // All remote keys are known. Start the data query thread.
        this->lastDataQuery = getNow() - this->dataQueryInterval;
        this->doQueryData = true;
        this->dataQueryThread.reset(
            new std::thread(&ControlWorker::dataQueryWorker, this));
        this->controlWorkerSubState =
            ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_REMOTE_STOPPED;
      }

      return true;
    }

    else if (ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_REMOTE_STOPPED ==
             this->controlWorkerSubState) {
      this->handleStatusPayload(response);

      // If this is a data response message, write the contents to the local
      // data manager.
      auto dataResponseMsg =
          dynamic_pointer_cast<DataResponsePayload>(response->getReadPaylod());
      if (!dataResponseMsg) {
        return true;
      }

      LOG(INFO) << "Received a DataResponseMessage: " << std::endl
                << dataResponseMsg->serialize();

      std::string key = std::to_string(response->getSource().id()) + "/" +
                        dataResponseMsg->key;
      this->dataManager->write(dataResponseMsg->timestamps, key,
                               dataResponseMsg->values);

      return true;

    }

    else {
      LOG(WARNING)
          << "Control worker does not expect responses in the current state.";
      return false;
    }
  }

  else if (DeviceStatus::OPERATING == this->workerState) {
    this->handleStatusPayload(response);
    return true;
  }

  else {
    LOG(WARNING)
        << "Control worker does not expect responses in the current state.";
    return false;
  }
}

std::string ControlWorker::getWorkerName() { return CONTROL_WORKER_TYPE_NAME; }

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
  this->connectionTimeout = Core::getNow();
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
    if (this->controlWorkerSubState == CONTROL_WORKER_SUBSTATE_INIT_REMOTE ||
        this->controlWorkerSubState == CONTROL_WORKER_SUBSTATE_CONF_REMOTE ||
        this->controlWorkerSubState == CONTROL_WORKER_SUBSTATE_REMOTE_RUNNING ||
        this->controlWorkerSubState == CONTROL_WORKER_SUBSTATE_REMOTE_STOPPED) {

      // Send a query state message to each remote id.
      for (auto &keyValuePair : this->remoteHostIds) {
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

  // If the device, the status payload originates from, is already known,
  // replace the entry, otherwise add an entry.
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

bool ControlWorker::setRemoteWorkerState(UserId remoteId, bool state) {
  // TODO: Do some sanity checks here.

  WriteDeviceTopic enableState = state ? WriteDeviceTopic::WRITE_TOPIC_RUN
                                       : WriteDeviceTopic::WRITE_TOPIC_STOP;
  this->pushMessageQueue(std::shared_ptr<DeviceMessage>(
      new WriteDeviceMessage(this->getUserId(), remoteId, enableState)));
  return true;
}

UserId ControlWorker::getPumpControllerId() {
  UserId retVal;
  for (auto keyValuePair : this->remoteHostIds) {
    if (get<1>(keyValuePair.second) == SENTRY_WORKER_TYPE_NAME) {
      retVal = keyValuePair.first;
      break;
    }
  }

  auto it = std::find_if(
      this->remoteHostIds.begin(), this->remoteHostIds.end(),
      [](std::pair<size_t, std::tuple<bool, std::string, DeviceType>> pair) {
        return std::get<2>(pair.second) == DeviceType::PUMP_CONTROLLER;
      });

  if (it != this->remoteHostIds.end()) {
    return it->first;
  } else {
    return UserId();
  }
}

UserId ControlWorker::getSpectrometerId() {
  UserId retVal;
  for (auto keyValuePair : this->remoteHostIds) {
    if (get<1>(keyValuePair.second) == SENTRY_WORKER_TYPE_NAME) {
      retVal = keyValuePair.first;
      break;
    }
  }

  auto it = std::find_if(
      this->remoteHostIds.begin(), this->remoteHostIds.end(),
      [](std::pair<size_t, std::tuple<bool, std::string, DeviceType>> pair) {
        return std::get<2>(pair.second) == DeviceType::IMPEDANCE_SPECTROMETER;
      });

  if (it != this->remoteHostIds.end()) {
    return it->first;
  } else {
    return UserId();
  }
}

void ControlWorker::dataQueryWorker() {
  while (this->doQueryData) {
    // Send a data query message to all known remote devices.
    TimePoint now = getNow();

    std::vector<std::shared_ptr<DeviceMessage>> dataQueryMessages;
    for (auto remoteDataKeyEntry : this->remoteDataKeys) {
      KeyMapping keyMapping = std::get<KeyMapping>(remoteDataKeyEntry.second);
      for (auto &key : keyMapping) {

        std::shared_ptr<WritePayload> dataRequestPayload(
            new RequestDataPayload(this->lastDataQuery, now, key.first));

        dataQueryMessages.emplace_back(std::shared_ptr<DeviceMessage>(
            new WriteDeviceMessage(this->getUserId(), remoteDataKeyEntry.first,
                                   Messages::WRITE_TOPIC_REQUEST_DATA,
                                   dataRequestPayload)));
      }
    }

    this->pushMessageQueue(dataQueryMessages);
    this->lastDataQuery = now;
    std::this_thread::sleep_for(this->dataQueryInterval);
  }
}

std::vector<std::tuple<TimePoint, ImpedanceSpectrum>>
ControlWorker::getSpectra(TimePoint from, TimePoint to) {
  // Get the id of the spectrometer.
  UserId spectrometerId = getSpectrometerId();

  // Get the data keys of the spectrometer and find the ones that refer to a
  // spectrum.
  KeyMapping keyMap =
      std::get<KeyMapping>(this->remoteDataKeys[spectrometerId.id()]);

  // Copy the keys to a spectrum dataset into a temporary key mapping.
  KeyMapping keyMapTemp;
  for (auto &keyValue : keyMap) {
    if (keyValue.second == DATAMANAGER_DATA_TYPE_SPECTRUM) {
      keyMapTemp[keyValue.first] = keyValue.second;
    }
  }

  // Get the key for the most recent spectrum.
  std::string targetKey;
  TimePoint largestTimestamp;
  for (auto &keyValue : keyMapTemp) {
    std::vector<std::string> split = Utilities::split(keyValue.first, '_');

    int year, month, day, hour, minute;
    sscanf_s(split.front().c_str(), "%4i%2i%2i%2i%2i", &year, &month, &day,
             &hour, &minute);
    TimePoint timestamp;
    timestamp += std::chrono::years(year) + std::chrono::months(month) +
                 std::chrono::days(day) + std::chrono::hours(hour) +
                 std::chrono::minutes(minute);

    if (timestamp > largestTimestamp) {
      targetKey = keyValue.first;
      largestTimestamp = timestamp;
    }
  }

  // At the control worker side, the user id number of the remote device is
  // prepended.
  targetKey = std::to_string(spectrometerId.id()) + "/" + targetKey;

  std::vector<TimePoint> timestamps;
  std::vector<Value> values;
  bool success =
      this->dataManager->read(from, to, targetKey, timestamps, values);
  std::vector<ImpedanceSpectrum> spectra;

  if (!success) {
    return std::vector<std::tuple<TimePoint, ImpedanceSpectrum>>();
  }

  // Transform std::vector<Value> to std::vector<ImpedanceSpectrum>
  std::vector<ImpedanceSpectrum> spectrum;
  std::transform(
      values.cbegin(), values.cend(), std::back_inserter(spectrum),
      [](Value value) { return std::get<ImpedanceSpectrum>(value); });

  // Glob timestamps and values together.
  std::vector<std::tuple<TimePoint, ImpedanceSpectrum>> retVal;
  std::transform(timestamps.cbegin(), timestamps.cend(), spectrum.cbegin(),
                 std::back_inserter(retVal),
                 [](TimePoint timepoint, ImpedanceSpectrum spectrum) {
                   return std::make_tuple(timepoint, spectrum);
                 });

  return retVal;
}
