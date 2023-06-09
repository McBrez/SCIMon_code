// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <common.hpp>
#include <device.hpp>
#include <isx3_init_payload.hpp>
#include <message_distributor.hpp>
#include <ob1_init_payload.hpp>
#include <request_data_payload.hpp>
#include <sentry_worker.hpp>

namespace Workers {
SentryWorker::SentryWorker()
    : initSubState(InitSubState::INIT_SUB_STATE_INVALID),
      pumpControllerState(DeviceStatus::UNKNOWN_DEVICE_STATUS),
      spectrometerState(DeviceStatus::UNKNOWN_DEVICE_STATUS),
      threadWaiting(true), runThread(false),
      workerThreadInterval(chrono::milliseconds(100)) {
  this->workerState = DeviceStatus::UNKNOWN_DEVICE_STATUS;
}

SentryWorker::~SentryWorker() {}

void SentryWorker::work(TimePoint timestamp) {
  LOG(INFO) << "SentryWorker starting up.";

  // Ensure the devices are in a defined state. Turn them both off.
  this->messageOut.push(shared_ptr<DeviceMessage>(
      new WriteDeviceMessage(this->self->getUserId(), this->spectrometer,
                             WriteDeviceTopic::WRITE_TOPIC_STOP)));
  this->messageOut.push(shared_ptr<DeviceMessage>(
      new WriteDeviceMessage(this->self->getUserId(), this->pumpController,
                             WriteDeviceTopic::WRITE_TOPIC_STOP)));

  while (this->runThread) {

    // Decide what to do, according to the configured state.
    if (SentryWorkerMode::SENTRY_WORKER_MODE_MANUAL ==
        this->configPayload->sentryWorkerMode) {

      // Nothing to do.
    }

    else if (SentryWorkerMode::SENTRY_WORKER_MODE_TIMER ==
             this->configPayload->sentryWorkerMode) {
      if (this->threadWaiting) {
        if (chrono::system_clock::now() > waitUntil) {
          LOG(INFO) << "Enabling pump and deactivating impedance measurement.";

          // Enable the pump ...
          this->messageOut.push(shared_ptr<DeviceMessage>(
              new WriteDeviceMessage(this->self->getUserId(),
                                     this->pumpController,
                                     WriteDeviceTopic::WRITE_TOPIC_RUN)));

          // ... disable the spectrometer ...
          this->messageOut.push(shared_ptr<DeviceMessage>(
              new WriteDeviceMessage(this->self->getUserId(),
                                     this->spectrometer,
                                     WriteDeviceTopic::WRITE_TOPIC_STOP)));

          this->threadWaiting = false;
        }
      } else {
        LOG(INFO) << "Disabling pump and activating impedance measurement.";

        // Disable the pump ...
        this->messageOut.push(shared_ptr<DeviceMessage>(new WriteDeviceMessage(
            this->self->getUserId(), this->pumpController,
            WriteDeviceTopic::WRITE_TOPIC_STOP)));

        // ... enable the spectrometer ...
        this->messageOut.push(shared_ptr<DeviceMessage>(
            new WriteDeviceMessage(this->self->getUserId(), this->spectrometer,
                                   WriteDeviceTopic::WRITE_TOPIC_RUN)));

        // Print out measurement results until on time passes.
        TimePoint now = chrono::system_clock::now();
        TimePoint until = now + this->configPayload->offTime;
        while (now < until) {

          this->isPayloadCacheMutex.lock();
          while (!this->isPayloadCache.empty()) {
            LOG(INFO) << this->isPayloadCache.front()->serialize();
            this->isPayloadCache.pop_front();
          }

          this->isPayloadCacheMutex.unlock();

          this_thread::sleep_for(chrono::milliseconds(100));
          now = chrono::system_clock::now();
        }

        this->threadWaiting = true;
      }
    }

    else {
    }

    this_thread::sleep_for(this->workerThreadInterval);
  }
}

bool SentryWorker::specificWrite(shared_ptr<WriteDeviceMessage> writeMsg) {

  // Get the payload and cast it down.
  shared_ptr<RequestDataPayload> requestDataPayload =
      dynamic_pointer_cast<RequestDataPayload>(writeMsg->getPayload());
  if (requestDataPayload) {
    

    return true;
  }

  return false;
}

list<shared_ptr<DeviceMessage>>
SentryWorker::specificRead(TimePoint timestamp) {
  return list<shared_ptr<DeviceMessage>>();
}

bool SentryWorker::handleResponse(shared_ptr<ReadDeviceMessage> response) {

  if (DeviceStatus::INITIALIZING == this->getState()) {
    // During INITIALIZING, it is waited until the governed devices become
    // operational. This happens in three configuration sub states: INIT ->
    // CONFIGURE -> OPERATING
    if (InitSubState::INIT_SUB_STATE_INIT == this->initSubState) {
      // Wait until both devices finished initializing.
      // Check the current response.
      auto statusPayload =
          dynamic_pointer_cast<StatusPayload>(response->getReadPaylod());
      if (!statusPayload) {
        // Only device status messages are expected right now. Return here.
        LOG(WARNING) << "SentryWorker got an unexpected message while in state "
                     << this->getState();

        return false;
      }

      // Is this message from the spectrometer?;
      if (response->getSource() == this->spectrometer) {
        // It is. Check if it finished initializing.
        if (statusPayload->getDeviceStatus() == DeviceStatus::INITIALIZED) {
          this->spectrometerState = DeviceStatus::INITIALIZED;
        } else {
          // Device is not yet ready. Resend the query state message.
          this->messageOut.push(
              shared_ptr<DeviceMessage>(new WriteDeviceMessage(
                  this->self->getUserId(), response->getSource(),
                  WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));

          return true;
        }
      }
      // Message is not from the spectrometer. Is it from the pump controller?
      else if (response->getSource() == this->pumpController) {
        // It is. Check if it finished initializing.
        if (statusPayload->getDeviceStatus() == DeviceStatus::INITIALIZED) {
          this->pumpControllerState = DeviceStatus::INITIALIZED;
        } else {
          // Device is not yet ready. Resend the query state message.
          this->messageOut.push(
              shared_ptr<DeviceMessage>(new WriteDeviceMessage(
                  this->self->getUserId(), response->getSource(),
                  WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));

          return true;
        }
      } else {
        // Unknown device. Ignore it.
        return false;
      }

      // Check if both devices are now initialized.
      if (this->spectrometerState == DeviceStatus::INITIALIZED &&
          this->pumpControllerState == DeviceStatus::INITIALIZED) {
        // Both devices are initialized. Send the configuration message to
        // both and transition to configure state.
        this->messageOut.push(shared_ptr<DeviceMessage>(
            new ConfigDeviceMessage(this->self->getUserId(), this->spectrometer,
                                    this->initPayload->isSpecConfPayload)));
        this->messageOut.push(shared_ptr<DeviceMessage>(new ConfigDeviceMessage(
            this->self->getUserId(), this->pumpController,
            this->initPayload->pumpControllerConfigPayload)));
        // Also send them state query messages. So that this method keeps
        // getting called.
        this->messageOut.push(shared_ptr<DeviceMessage>(
            new WriteDeviceMessage(this->self->getUserId(), spectrometer,
                                   WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));
        this->messageOut.push(shared_ptr<DeviceMessage>(
            new WriteDeviceMessage(this->self->getUserId(), pumpController,
                                   WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));

        this->initSubState = InitSubState::INIT_SUB_STATE_CONFIGURE;
      }
    }

    else if (InitSubState::INIT_SUB_STATE_CONFIGURE == this->initSubState) {
      // Wait until both devices finished configuring.
      // Check the current response.
      auto statusPayload =
          dynamic_pointer_cast<StatusPayload>(response->getReadPaylod());
      if (!statusPayload) {
        // Only device status messages are expected right now. Return here.
        return false;
      }

      // Is this message from the spectrometer?;
      if (response->getSource() == this->spectrometer) {
        // It is. Check if it finished initializing.
        if (statusPayload->getDeviceStatus() == DeviceStatus::IDLE) {
          this->spectrometerState = DeviceStatus::IDLE;

          return true;
        } else {
          // Device is not yet ready. Resend the query state message.
          this->messageOut.push(
              shared_ptr<DeviceMessage>(new WriteDeviceMessage(
                  this->self->getUserId(), response->getSource(),
                  WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));

          return true;
        }
      }

      // Message is not from the spectrometer. Is it from the pump controller?
      else if (response->getSource() == this->pumpController) {
        // It is. Check if it finished initializing.
        if (statusPayload->getDeviceStatus() == DeviceStatus::IDLE) {
          this->pumpControllerState = DeviceStatus::IDLE;

          return true;
        } else {
          // Device is not yet ready. Resend the query state message.
          this->messageOut.push(
              shared_ptr<DeviceMessage>(new WriteDeviceMessage(
                  this->self->getUserId(), response->getSource(),
                  WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));

          return true;
        }
      }

      else {
        // Unknown device. Ignore it.
        return false;
      }

      // Check if both devices are now finished configuring
      if (this->spectrometerState == DeviceStatus::IDLE &&
          this->pumpControllerState == DeviceStatus::IDLE) {
        // Both devices have been configured successfully. Set the state
        // accordingly.
        this->initSubState = InitSubState::INIT_SUB_STATE_READY;
        this->workerState = DeviceStatus::INITIALIZED;

        return true;
      }

      else {
        // Responses are not expected in all other states. Just return true.
        return true;
      }
    }
  }

  else if (DeviceStatus::OPERATING == this->getState()) {
    // During OPERATING, received messages from the devices are written to
    // cache.
    if (!response->getReadPaylod()) {
      return false;
    }

    auto castedResponse =
        dynamic_pointer_cast<IsPayload>(response->getReadPaylod());
    if (!castedResponse) {
      return false;
    }

    this->isPayloadCacheMutex.lock();
    this->isPayloadCache.push_back(castedResponse);
    this->isPayloadCacheMutex.unlock();

    return true;
  }

  else {
    return false;
  }
}

bool SentryWorker::start() {
  // Check if worker is in the correct state.
  if (DeviceStatus::IDLE != this->workerState) {
    LOG(WARNING) << "SentryWorker received start message in state "
                 << this->workerState;

    return false;
  }

  // Start the worker.
  this->runThread = true;
  this->workerThread.reset(
      new thread(&SentryWorker::work, this, chrono::system_clock::now()));
  this->workerState = DeviceStatus::OPERATING;

  return true;
}

bool SentryWorker::stop() {
  // Check if worker is in the correct state.
  if (DeviceStatus::OPERATING != this->workerState) {
    LOG(WARNING) << "SentryWorker received stop message in state "
                 << this->workerState;

    return false;
  }

  // Stop the worker.
  this->runThread = false;
  this->workerThread->join();
  this->workerState = DeviceStatus::IDLE;

  return true;
}

bool SentryWorker::initialize(shared_ptr<InitPayload> initPayload) {

  // Check if this is the correct payload type.
  auto sentryInitPayload = dynamic_pointer_cast<SentryInitPayload>(initPayload);
  if (!sentryInitPayload) {
    LOG(WARNING) << "SentryWorker received malformed init payload. This will "
                    "be ignored.";

    return false;
  }

  // Reset worker state.
  this->workerState = DeviceStatus::UNKNOWN_DEVICE_STATUS;
  this->pumpController = nullptr;
  this->spectrometer = nullptr;

  // Check if an pump controller and a impedance spectrometer are present.
  list<shared_ptr<StatusPayload>> participants =
      this->messageDistributor->getStatus();
  UserId spectrometerTemp;
  UserId pumpControllerTemp;
  for (auto participant : participants) {
    if (participant->getDeviceType() == DeviceType::IMPEDANCE_SPECTROMETER) {
      spectrometerTemp = participant->getDeviceId();
    }
    if (participant->getDeviceType() == DeviceType::PUMP_CONTROLLER) {
      pumpControllerTemp = participant->getDeviceId();
    }
  }
  if (!spectrometerTemp || !pumpControllerTemp) {
    LOG(ERROR) << "SentryWorker did not find a spectrometer and a pump "
                  "controller. Can not continue.";

    return false;
  }
  // Remember the both devices and the sentry init payload.
  this->spectrometer = spectrometerTemp;
  this->pumpController = pumpControllerTemp;
  this->initPayload = sentryInitPayload;

  // Set the worker state to INITIALIZING and forward the init messages to the
  // corresponding devices.
  this->workerState = DeviceStatus::INITIALIZING;
  this->messageOut.push(shared_ptr<DeviceMessage>(
      new InitDeviceMessage(this->self->getUserId(), spectrometer,
                            this->initPayload->isSpecInitPayload)));
  this->messageOut.push(shared_ptr<DeviceMessage>(
      new InitDeviceMessage(this->self->getUserId(), pumpController,
                            this->initPayload->pumpControllerInitPayload)));
  // Immediatelly send a state query message to the devices.
  this->messageOut.push(shared_ptr<DeviceMessage>(
      new WriteDeviceMessage(this->self->getUserId(), this->spectrometer,
                             WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));
  this->messageOut.push(shared_ptr<DeviceMessage>(
      new WriteDeviceMessage(this->self->getUserId(), this->pumpController,
                             WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));

  // Set the configure sub state.
  this->initSubState = InitSubState::INIT_SUB_STATE_INIT;

  // The init message have now been sent. The logic continues in
  // handleResponse().
  return true;
}

bool SentryWorker::configure(shared_ptr<ConfigurationPayload> configPayload) {
  // Check if the worker is in the correct state. Configuration is only
  // possible, when in INIT.
  if (this->getState() != DeviceStatus::INITIALIZED) {
    LOG(WARNING) << "SentryWorker received a configuration message, while it "
                    "was in state "
                 << this->workerState;

    return false;
  }

  // Try to downcast the payload.
  auto sentryConfigPayload =
      dynamic_pointer_cast<SentryConfigPayload>(configPayload);
  if (!sentryConfigPayload) {
    LOG(WARNING) << "SentryWorker received a malformed configuration message.";

    return false;
  }

  this->configPayload = sentryConfigPayload;
  this->workerState = DeviceStatus::IDLE;

  return true;
}

bool SentryWorker::write(shared_ptr<HandshakeMessage> writeMsg) {
  // Sentry worker does not react to handshake messages.
  LOG(WARNING) << "Sentry worker received handshake message. This will be "
                  "ignored.";
  return false;
}

string SentryWorker::getWorkerName() { return SENTRY_WORKER_TYPE_NAME; }

} // namespace Workers