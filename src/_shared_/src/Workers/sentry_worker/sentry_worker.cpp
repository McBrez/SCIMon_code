// 3rd party includes
#include <easylogging++.h>

// Project includes
#include <device.hpp>
#include <device_status_message.hpp>
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
  this->messageOut.push(shared_ptr<DeviceMessage>(new WriteDeviceMessage(
      this->self, this->spectrometer,
      WriteDeviceTopic::WRITE_TOPIC_STOP)));
  this->messageOut.push(shared_ptr<DeviceMessage>(new WriteDeviceMessage(
      this->self, this->pumpController,
      WriteDeviceTopic::WRITE_TOPIC_STOP)));

  while (this->runThread) {
    if (this->threadWaiting) {
      LOG(INFO) << "Enabling pump and deactivating impedance measurement.";

      // Enable the pump ...
      this->messageOut.push(shared_ptr<DeviceMessage>(new WriteDeviceMessage(
          this->self, this->pumpController,
          WriteDeviceTopic::WRITE_TOPIC_RUN)));

      // ... disable the spectrometer ...
      this->messageOut.push(shared_ptr<DeviceMessage>(new WriteDeviceMessage(
          this->self, this->spectrometer,
          WriteDeviceTopic::WRITE_TOPIC_STOP)));

      // ... and wait for a specified period.
      this_thread::sleep_until(chrono::system_clock::now() +
                               this->initPayload->offTime);
      this->threadWaiting = false;
    } else {
      LOG(INFO) << "Disabling pump and activating impedance measurement.";

      // Disable the pump ...
      this->messageOut.push(shared_ptr<DeviceMessage>(new WriteDeviceMessage(
          this->self, this->pumpController,
          WriteDeviceTopic::WRITE_TOPIC_STOP)));

      // ... enable the spectrometer ...
      this->messageOut.push(shared_ptr<DeviceMessage>(new WriteDeviceMessage(
          this->self, this->spectrometer,
          WriteDeviceTopic::WRITE_TOPIC_RUN)));

      // Print out measurement results until on time passes.
      TimePoint now = chrono::system_clock::now();
      TimePoint until = now + this->initPayload->onTime;
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
      auto deviceStatusMessage =
          dynamic_pointer_cast<DeviceStatusMessage>(response);
      if (!deviceStatusMessage) {
        // Only device status messages are expected right now. Return here.
        return false;
      }

      // Is this message from the spectrometer?;
      if (deviceStatusMessage->getSource() == this->spectrometer) {
        // It is. Check if it finished initializing.
        if (deviceStatusMessage->getDeviceStatus() ==
            DeviceStatus::INITIALIZED) {
          this->spectrometerState = DeviceStatus::INITIALIZED;
        } else {
          // Device is not yet ready. Resend the query state message.
          this->messageOut.push(
              shared_ptr<DeviceMessage>(new WriteDeviceMessage(
                  this->self, response->getSource(),
                  WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));
        }
      }

      // Message is not from the spectrometer. Is it from the pump controller?
      else if (deviceStatusMessage->getSource() == this->pumpController) {
        // It is. Check if it finished initializing.
        if (deviceStatusMessage->getDeviceStatus() ==
            DeviceStatus::INITIALIZED) {
          this->pumpControllerState = DeviceStatus::INITIALIZED;
        } else {
          // Device is not yet ready. Resend the query state message.
          this->messageOut.push(
              shared_ptr<DeviceMessage>(new WriteDeviceMessage(
                  this->self, response->getSource(),
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
        this->messageOut.push(shared_ptr<DeviceMessage>(new ConfigDeviceMessage(
            this->self, this->spectrometer,
            this->initPayload->isx3IsConfigPayload)));
        this->messageOut.push(shared_ptr<DeviceMessage>(
            new ConfigDeviceMessage(this->self,
                                    this->pumpController, nullptr)));

        this->configureSubState = ConfigureSubState::CONF_SUB_STATE_CONFIGURE;
      }
    }

    else if (ConfigureSubState::CONF_SUB_STATE_CONFIGURE ==
             this->configureSubState) {
      // Wait until both devices finished configuring.
      // Check the current response.
      auto deviceStatusMessage =
          dynamic_pointer_cast<DeviceStatusMessage>(response);
      if (!deviceStatusMessage) {
        // Only device status messages are expected right now. Return here.
        return false;
      }

      // Is this message from the spectrometer?;
      if (deviceStatusMessage->getSource() == this->spectrometer) {
        // It is. Check if it finished initializing.
        if (deviceStatusMessage->getDeviceStatus() == DeviceStatus::IDLE) {
          this->spectrometerState = DeviceStatus::IDLE;
        } else {
          // Device is not yet ready. Resend the query state message.
          this->messageOut.push(
              shared_ptr<DeviceMessage>(new WriteDeviceMessage(
                  this->self, response->getSource(),
                  WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));
        }
      }

      // Message is not from the spectrometer. Is it from the pump controller?
      else if (deviceStatusMessage->getSource() == this->pumpController) {
        // It is. Check if it finished initializing.
        if (deviceStatusMessage->getDeviceStatus() == DeviceStatus::IDLE) {
          this->pumpControllerState = DeviceStatus::IDLE;
        } else {
          // Device is not yet ready. Resend the query state message.
          this->messageOut.push(
              shared_ptr<DeviceMessage>(new WriteDeviceMessage(
                  this->self, response->getSource(),
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
        this->messageOut.push(shared_ptr<DeviceMessage>(new WriteDeviceMessage(
            this->self, this->spectrometer,
            WriteDeviceTopic::WRITE_TOPIC_RUN)));
        this->messageOut.push(shared_ptr<DeviceMessage>(new WriteDeviceMessage(
            this->self, this->pumpController,
            WriteDeviceTopic::WRITE_TOPIC_RUN)));

        this->configureSubState = ConfigureSubState::CONF_SUB_STATE_STARTED;
        if (this->initPayload->autoStart) {
          this->workerState = DeviceStatus::OPERATING;
          this->runThread = true;
          this->workerThread.reset(new thread(&SentryWorker::work, this,
                                              chrono::system_clock::now()));
        } else {
          this->workerState = DeviceStatus::IDLE;
        }
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
  list<shared_ptr<MessageInterface>> participants =
      this->messageDistributor->getParticipants();
  shared_ptr<Device> spectrometerTemp = nullptr;
  shared_ptr<Device> pumpControllerTemp = nullptr;
  for (auto participant : participants) {
    auto device = dynamic_pointer_cast<Device>(participant);
    if (!device) {
      continue;
    }

    if (device->getDeviceType() == DeviceType::IMPEDANCE_SPECTROMETER) {
      spectrometerTemp = device;
    }
    if (device->getDeviceType() == DeviceType::PUMP_CONTROLLER) {
      pumpControllerTemp = device;
    }
  }
  if (!spectrometerTemp || !pumpControllerTemp) {
    return false;
  }

  // Remember the both devices.
  this->spectrometer = spectrometerTemp;
  this->pumpController = pumpControllerTemp;

  // Send an init message to them.
  this->messageOut.push(shared_ptr<DeviceMessage>(new InitDeviceMessage(
      this->self, spectrometer, this->initPayload->isx3InitPayload)));
  this->messageOut.push(shared_ptr<DeviceMessage>(new InitDeviceMessage(
      this->self, pumpController, this->initPayload->ob1InitPayload)));

  // Immediatelly send a state query message to the devices.
  this->messageOut.push(shared_ptr<DeviceMessage>(new WriteDeviceMessage(
      this->self, spectrometer, WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));
  this->messageOut.push(shared_ptr<DeviceMessage>(new WriteDeviceMessage(
      this->self, pumpController, WriteDeviceTopic::WRITE_TOPIC_QUERY_STATE)));

  // Set the configure sub state.
  this->configureSubState = ConfigureSubState::CONF_SUB_STATE_INIT;

  // Configuration logic continues in handleResponse().
  return true;
}

} // namespace Workers