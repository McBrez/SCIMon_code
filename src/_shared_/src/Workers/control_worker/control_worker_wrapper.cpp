// 3rd party includes
#include <Elveflow64_shim.h>

// Project includes
#include <control_worker_wrapper.hpp>
#include <isx3_init_payload.hpp>
#include <isx3_is_conf_payload.hpp>
#include <message_factory.hpp>
#include <ob1_conf_payload.hpp>
#include <ob1_init_payload.hpp>

using namespace Workers;
using namespace Messages;
using namespace Devices;

ControlWorkerWrapper::ControlWorkerWrapper(int messageDistributorInterval,
                                           QObject *parent)
    : QObject(parent),
      messageDistributor(new MessageDistributor(messageDistributorInterval)),
      networkWorker(new NetworkWorker()), controlWorker(new ControlWorker()),
      messageThread(nullptr), doPeriodicActions(true),
      recentSpectrumQueryTimestamp() {
  // Init the message factory.
  MessageFactory::createInstace(
      {std::shared_ptr<PayloadDecoder>(new Isx3PayloadDecoder()),
       std::shared_ptr<PayloadDecoder>(new Ob1PayloadDecoder()),
       std::shared_ptr<PayloadDecoder>(new SentryPayloadDecoder())});

  // Add the workers to the distributor.
  messageDistributor->addParticipant(this->networkWorker);
  messageDistributor->addParticipant(this->controlWorker);

  // Start the message thread.
  this->messageThread.reset(
      new std::thread(&MessageDistributor::run, messageDistributor));

  // Immediatelly send a init message to the control worker.
  this->messageDistributor->takeMessage(
      std::shared_ptr<DeviceMessage>(new InitDeviceMessage(
          UserId(), this->controlWorker->getUserId(), nullptr)));
}

void ControlWorkerWrapper::startStateQuery() {
  // Start the periodic actions thread.
  this->periodicActionsThread.reset(
      new std::thread(&ControlWorkerWrapper::periodicActionsWorker, this));
}

ControlWorkerWrapper::~ControlWorkerWrapper() { this->shutdown(); }

void ControlWorkerWrapper::shutdown() {
  if (this->messageDistributor->isRunning()) {
    this->messageDistributor->stop();
    this->messageThread->join();
  }

  this->doPeriodicActions = false;
  if (this->periodicActionsThread) {
    this->periodicActionsThread->join();
    this->periodicActionsThread.release();
  }
}

void ControlWorkerWrapper::startConnection(QString ip, int port) {
  this->controlWorker->startConnect(ip.toStdString(), port);
}

void ControlWorkerWrapper::periodicActionsWorker() {

  DeviceStatus oldWorkerState = DeviceStatus::UNKNOWN_DEVICE_STATUS;
  ControlWorkerSubState oldWorkerSubState =
      ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_IVALID;
  std::list<std::shared_ptr<StatusPayload>> remoteStatesCache;
  while (this->doPeriodicActions) {
    // Check the worker state.
    DeviceStatus newWorkerState = this->controlWorker->getDeviceStatus();
    if (newWorkerState != oldWorkerState) {
      emit stateChanged(oldWorkerState, newWorkerState);
      oldWorkerState = newWorkerState;
    }

    // Check the sub state.
    ControlWorkerSubState newWorkerSubState =
        this->controlWorker->getControlWorkerState();
    if (newWorkerSubState != oldWorkerSubState) {
      emit subStateChanged(oldWorkerSubState, newWorkerSubState);
      oldWorkerSubState = newWorkerSubState;
    }

    // Check the remote states.
    std::list<std::shared_ptr<StatusPayload>> remoteStates =
        this->controlWorker->getRemoteStatus();
    if (remoteStates != remoteStatesCache) {
      emit remoteStateChanged(
          QList<std::shared_ptr<StatusPayload>>(remoteStatesCache.begin(),
                                                remoteStatesCache.end()),
          QList<std::shared_ptr<StatusPayload>>(remoteStates.begin(),
                                                remoteStates.end()));
      remoteStatesCache = remoteStates;
    }

    // Query for new spectrum data.
    TimePoint now = getNow();
    std::vector<std::tuple<TimePoint, ImpedanceSpectrum>> newIsData =
        this->controlWorker->getSpectra(this->recentSpectrumQueryTimestamp,
                                        now);
    if (!newIsData.empty()) {
      emit newSpectrumData(newIsData);
      this->recentSpectrumQueryTimestamp = now;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

QString ControlWorkerWrapper::controlWorkerSubStateToString(
    ControlWorkerSubState subState) {

  if (ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_CONNECTED == subState) {
    return QString("Connected");
  }

  else if (ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_CONNECTING ==
           subState) {
    return QString("Connecting");
  }

  else if (ControlWorkerSubState::
               CONTROL_WORKER_SUBSTATE_WAITING_FOR_CONNECTION == subState) {
    return QString("Waiting to connect.");
  }

  else if (ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_CONF_EXPLORE ==
           subState) {
    return QString("Exploring ...");
  }

  else if (ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_CONF_REMOTE ==
           subState) {
    return QString("Configuring ...");
  }

  else if (ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_REMOTE_RUNNING ==
           subState) {
    return QString("Running");
  }

  else if (ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_REMOTE_STOPPED ==
           subState) {
    return QString("Stopped");
  }

  else {
    return QString("INVALID");
  }
}

void ControlWorkerWrapper::startConfig() {

  std::shared_ptr<SentryInitPayload> initPayload(new SentryInitPayload(
      new Isx3InitPayload("COM3", 115200),
      new Isx3IsConfPayload(
          10.0, 1000.0, 10, 0,
          std::map<ChannelFunction, int>{{ChannelFunction::CHAN_FUNC_CP, 0x0C},
                                         {ChannelFunction::CHAN_FUNC_RP, 39},
                                         {ChannelFunction::CHAN_FUNC_WP, 0x0F},
                                         {ChannelFunction::CHAN_FUNC_WS, 39}},
          IsScale::LINEAR_SCALE,
          MeasurmentConfigurationRange::MEAS_CONFIG_RANGE_10MA,
          MeasurmentConfigurationChannel::MEAS_CONFIG_CHANNEL_EXT_PORT,
          MeasurementConfiguration::MEAS_CONFIG_2_POINT, 1.0, 1.0),
      new Ob1InitPayload("01FB0FA3",
                         std::make_tuple(Z_regulator_type_m1000_1000_mbar,
                                         Z_regulator_type_none,
                                         Z_regulator_type_none,
                                         Z_regulator_type_none)),
      new Ob1ConfPayload()));
  std::shared_ptr<SentryConfigPayload> configPayload(new SentryConfigPayload(
      SentryWorkerMode::SENTRY_WORKER_MODE_MANUAL, Duration(), Duration()));

  this->controlWorker->startConfig(initPayload, configPayload);
}

void ControlWorkerWrapper::start() { this->controlWorker->start(); }

void ControlWorkerWrapper::stop() { this->controlWorker->stop(); }

void ControlWorkerWrapper::start_isx3() {
  // Find the ISX3 device.
  std::list<std::shared_ptr<StatusPayload>> statusList =
      this->controlWorker->getRemoteStatus();

  for (auto it : statusList) {
    if (it->getDeviceType() == DeviceType::IMPEDANCE_SPECTROMETER) {
      this->controlWorker->setRemoteWorkerState(it->getDeviceId(), true);

      return;
    }
  }
}

void ControlWorkerWrapper::stop_isx3() {
  // Find the ISX3 device.
  std::list<std::shared_ptr<StatusPayload>> statusList =
      this->controlWorker->getRemoteStatus();

  for (auto it : statusList) {
    if (it->getDeviceType() == DeviceType::IMPEDANCE_SPECTROMETER) {
      this->controlWorker->setRemoteWorkerState(it->getDeviceId(), false);

      return;
    }
  }
}

void ControlWorkerWrapper::start_ob1() {
  // Find the OB1 device.
  std::list<std::shared_ptr<StatusPayload>> statusList =
      this->controlWorker->getRemoteStatus();

  for (auto it : statusList) {
    if (it->getDeviceType() == DeviceType::PUMP_CONTROLLER) {
      this->controlWorker->setRemoteWorkerState(it->getDeviceId(), true);

      return;
    }
  }
}

void ControlWorkerWrapper::stop_ob1() {
  // Find the OB1 device.
  std::list<std::shared_ptr<StatusPayload>> statusList =
      this->controlWorker->getRemoteStatus();

  for (auto it : statusList) {
    if (it->getDeviceType() == DeviceType::PUMP_CONTROLLER) {
      this->controlWorker->setRemoteWorkerState(it->getDeviceId(), false);

      return;
    }
  }
}

void ControlWorkerWrapper::setPressure(int channel, double pressure) {
  this->controlWorker->setPressure(channel, pressure);
}
