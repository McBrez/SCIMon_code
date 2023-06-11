// Project includes
#include <control_worker_wrapper.hpp>
#include <message_factory.hpp>

// REMOV ME
#include <qthread>

using namespace Workers;
using namespace Messages;

ControlWorkerWrapper::ControlWorkerWrapper(int messageDistributorInterval,
                                           QObject *parent)
    : QObject(parent),
      messageDistributor(new MessageDistributor(messageDistributorInterval)),
      networkWorker(new NetworkWorker()), controlWorker(new ControlWorker()),
      messageThread(nullptr), doPeriodicActions(true) {
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

void ControlWorkerWrapper::start() {
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
  this->periodicActionsThread->join();
  this->periodicActionsThread.release();
}

void ControlWorkerWrapper::startConnection(QString ip, int port) {
  this->controlWorker->startConnect(ip.toStdString(), port);
}

void ControlWorkerWrapper::periodicActionsWorker() {

  DeviceStatus oldWorkerState = DeviceStatus::UNKNOWN_DEVICE_STATUS;
  ControlWorkerSubState oldWorkerSubState =
      ControlWorkerSubState::CONTROL_WORKER_SUBSTATE_IVALID;

  while (this->doPeriodicActions) {
    // Check the worker state.
    DeviceStatus newWorkerState = this->controlWorker->getState();
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

  else {
    return QString("INVALID");
  }
}
