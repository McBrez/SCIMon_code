// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <common.hpp>
#include <device.hpp>
#include <isx3_init_payload.hpp>
#include <message_distributor.hpp>
#include <ob1_init_payload.hpp>
#include <sentry_worker.hpp>

namespace Workers {
SentryWorker::SentryWorker()
    : configureSubState(ConfigureSubState::CONF_SUB_STATE_INVALID),
      pumpControllerState(DeviceStatus::UNKNOWN_DEVICE_STATUS),
      spectrometerState(DeviceStatus::UNKNOWN_DEVICE_STATUS),
      threadWaiting(true), runThread(false) {}

SentryWorker::~SentryWorker() {}

void SentryWorker::work(TimePoint timestamp) {
  // Ensure the devices are in a defined state. Turn them both off.
  this->messageOut.push(shared_ptr<DeviceMessage>(
      new WriteDeviceMessage(this->self->getUserId(), this->spectrometer,
                             WriteDeviceTopic::WRITE_TOPIC_STOP)));
  this->messageOut.push(shared_ptr<DeviceMessage>(
      new WriteDeviceMessage(this->self->getUserId(), this->pumpController,
                             WriteDeviceTopic::WRITE_TOPIC_STOP)));

  while (this->runThread) {
    if (this->threadWaiting) {
      LOG(INFO) << "Enabling pump and deactivating impedance measurement.";

      // Enable the pump ...
      this->messageOut.push(shared_ptr<DeviceMessage>(
          new WriteDeviceMessage(this->self->getUserId(), this->pumpController,
                                 WriteDeviceTopic::WRITE_TOPIC_RUN)));

      // ... disable the spectrometer ...
      this->messageOut.push(shared_ptr<DeviceMessage>(
          new WriteDeviceMessage(this->self->getUserId(), this->spectrometer,
                                 WriteDeviceTopic::WRITE_TOPIC_STOP)));

      // ... and wait for a specified period.
      // this_thread::sleep_until(chrono::system_clock::now()
      // +this->initPayload->offTime);
      this->threadWaiting = false;
    } else {
      LOG(INFO) << "Disabling pump and activating impedance measurement.";

      // Disable the pump ...
      this->messageOut.push(shared_ptr<DeviceMessage>(
          new WriteDeviceMessage(this->self->getUserId(), this->pumpController,
                                 WriteDeviceTopic::WRITE_TOPIC_STOP)));

      // ... enable the spectrometer ...
      this->messageOut.push(shared_ptr<DeviceMessage>(
          new WriteDeviceMessage(this->self->getUserId(), this->spectrometer,
                                 WriteDeviceTopic::WRITE_TOPIC_RUN)));

      // Print out measurement results until on time passes.
      TimePoint now = chrono::system_clock::now();
      TimePoint until;
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
}

bool SentryWorker::specificWrite(shared_ptr<WriteDeviceMessage> writeMsg) {
  return false;
}

list<shared_ptr<DeviceMessage>>
SentryWorker::specificRead(TimePoint timestamp) {
  return list<shared_ptr<DeviceMessage>>();
}

bool SentryWorker::handleResponse(shared_ptr<ReadDeviceMessage> response) {

  if (DeviceStatus::CONFIGURING == this->getState()) {
    // During CONFIGURING, it is waited until the governed devices become
    // operational. This happens in three configuration sub states: INIT ->
    // CONFIGURE -> OPERATING
    if (ConfigureSubState::CONF_SUB_STATE_INIT == this->configureSubState) {
      // Wait until both devices finished initializing.
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
        if (statusPayload->getDeviceStatus() == DeviceStatus::INITIALIZED) {
          this->spectrometerState = DeviceStatus::INITIALIZED;
        } else {
          // Device is not yet ready. Resend the query state message.
          this->messageOut.push(
              shared_ptr<DeviceMessage>(new WriteDeviceMessage(
                  this->self->getUserId(), response->getSource(),
                  WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));
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
        }
      }

      else {
        // Unknown device. Ignore it.
        return false;
      }

      // Check if both devices are now initialized.
      if (this->spectrometerState == DeviceStatus::INITIALIZED &&
          this->pumpControllerState == DeviceStatus::INITIALIZED) {
// Both devices are initialized. Send the configuration message to both
// and transition to configure state.
#if 0
        this->messageOut.push(shared_ptr<DeviceMessage>(
            new ConfigDeviceMessage(this->self->getUserId(), this->spectrometer,
                                    this->initPayload->isx3IsConfigPayload)));
#endif
        this->messageOut.push(shared_ptr<DeviceMessage>(new ConfigDeviceMessage(
            this->self->getUserId(), this->pumpController, nullptr)));
        // Also send them state query messages. So that this method keeps
        // getting called.
        this->messageOut.push(shared_ptr<DeviceMessage>(
            new WriteDeviceMessage(this->self->getUserId(), spectrometer,
                                   WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));
        this->messageOut.push(shared_ptr<DeviceMessage>(
            new WriteDeviceMessage(this->self->getUserId(), pumpController,
                                   WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));

        this->configureSubState = ConfigureSubState::CONF_SUB_STATE_CONFIGURE;
      }
    }

    else if (ConfigureSubState::CONF_SUB_STATE_CONFIGURE ==
             this->configureSubState) {
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
        } else {
          // Device is not yet ready. Resend the query state message.
          this->messageOut.push(
              shared_ptr<DeviceMessage>(new WriteDeviceMessage(
                  this->self->getUserId(), response->getSource(),
                  WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));
        }
      }

      // Message is not from the spectrometer. Is it from the pump controller?
      else if (response->getSource() == this->pumpController) {
        // It is. Check if it finished initializing.
        if (statusPayload->getDeviceStatus() == DeviceStatus::IDLE) {
          this->pumpControllerState = DeviceStatus::IDLE;
        } else {
          // Device is not yet ready. Resend the query state message.
          this->messageOut.push(
              shared_ptr<DeviceMessage>(new WriteDeviceMessage(
                  this->self->getUserId(), response->getSource(),
                  WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));
        }
      }

      else {
        // Unknown device. Ignore it.
        return false;
      }

      // Check if both devices are now finished configuring
      if (this->spectrometerState == DeviceStatus::IDLE &&
          this->pumpControllerState == DeviceStatus::IDLE) {
        // Both devices have been configured successfully. Send the
        // start message to both and transition to started state.
        this->messageOut.push(shared_ptr<DeviceMessage>(
            new WriteDeviceMessage(this->self->getUserId(), this->spectrometer,
                                   WriteDeviceTopic::WRITE_TOPIC_RUN)));
        this->messageOut.push(shared_ptr<DeviceMessage>(new WriteDeviceMessage(
            this->self->getUserId(), this->pumpController,
            WriteDeviceTopic::WRITE_TOPIC_RUN)));

        this->configureSubState = ConfigureSubState::CONF_SUB_STATE_STARTED;

        this->workerState = DeviceStatus::OPERATING;
        this->runThread = true;
        this->workerThread.reset(
            new thread(&SentryWorker::work, this, chrono::system_clock::now()));
      }
    }

    else if (ConfigureSubState::CONF_SUB_STATE_STARTING ==
             this->configureSubState) {
    }

    else {
      // Responses are not expected in all other states. Just return true.
      return true;
    }
  }

  else if (DeviceStatus::OPERATING == this->getState()) {
    // During messages received data is forwarded to the worker thread.
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

bool SentryWorker::start() { return true; }

bool SentryWorker::stop() { return false; }

bool SentryWorker::initialize(shared_ptr<InitPayload> initPayload) {

  this->workerState = DeviceStatus::UNKNOWN_DEVICE_STATUS;

  // Check if this is the correct payload type.
  auto sentryInitPayload = dynamic_pointer_cast<SentryInitPayload>(initPayload);
  if (!sentryInitPayload) {
    return false;
  }

  this->pumpController = nullptr;
  this->spectrometer = nullptr;

  this->initPayload = sentryInitPayload;
  this->workerState = DeviceStatus::INITIALIZED;
  return true;
}

bool SentryWorker::configure(shared_ptr<ConfigurationPayload> configPayload) {
  // Check if the worker is in the correct state. Configuration is only
  // possible, when in INIT.
  if (this->getState() != DeviceStatus::INITIALIZED) {
    return false;
  }

  this->workerState = DeviceStatus::CONFIGURING;
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
    return false;
  }

  // Remember the both devices.
  this->spectrometer = spectrometerTemp;
  this->pumpController = pumpControllerTemp;

// Send an init message to them.
#if 0
  this->messageOut.push(shared_ptr<DeviceMessage>(
      new InitDeviceMessage(this->self->getUserId(), spectrometer,
                            this->initPayload)));
  this->messageOut.push(shared_ptr<DeviceMessage>(
      new InitDeviceMessage(this->self->getUserId(), pumpController,
                            this->initPayload->ob1InitPayload)));
#endif
  // Immediatelly send a state query message to the devices.
  this->messageOut.push(shared_ptr<DeviceMessage>(
      new WriteDeviceMessage(this->self->getUserId(), spectrometer,
                             WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));
  this->messageOut.push(shared_ptr<DeviceMessage>(
      new WriteDeviceMessage(this->self->getUserId(), pumpController,
                             WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));

  // Set the configure sub state.
  this->configureSubState = ConfigureSubState::CONF_SUB_STATE_INIT;

  // Configuration logic continues in handleResponse().
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