// Qt includes
#include "dialog.h"

// UI includes
#include "ui_dialog.h"

// Project includes
#include <device.hpp>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog), controlWorkerWrapper(100, parent) {
  ui->setupUi(this);

  this->ui->tbl_control_workers->setRowCount(1);
  this->ui->tbl_control_workers->setColumnCount(2);

  QObject::connect(this, &Dialog::finished, &this->controlWorkerWrapper,
                   &ControlWorkerWrapper::shutdown);

  QObject::connect(this->ui->btn_connect, &QPushButton::clicked,
                   &this->controlWorkerWrapper, [this]() {
                     this->controlWorkerWrapper.startConnection(
                         this->ui->txt_ip->text(),
                         this->ui->txt_port->text().toInt());
                   });
  QObject::connect(this->ui->btn_configControl, &QPushButton::clicked,
                   &this->controlWorkerWrapper,
                   &ControlWorkerWrapper::startConfig);

  QObject::connect(&this->controlWorkerWrapper,
                   &ControlWorkerWrapper::stateChanged, this,
                   &Dialog::onControlStateChanged, Qt::QueuedConnection);

  QObject::connect(&this->controlWorkerWrapper,
                   &ControlWorkerWrapper::subStateChanged, this,
                   &Dialog::onControlSubStateChanged, Qt::QueuedConnection);

  QObject::connect(&this->controlWorkerWrapper,
                   &ControlWorkerWrapper::remoteStateChanged, this,
                   &Dialog::onRemoteStatesChanged, Qt::QueuedConnection);

  QObject::connect(this->ui->btn_start, &QPushButton::clicked,
                   &this->controlWorkerWrapper, &ControlWorkerWrapper::start);
  QObject::connect(this->ui->btn_stop, &QPushButton::clicked,
                   &this->controlWorkerWrapper, &ControlWorkerWrapper::stop);

  this->controlWorkerWrapper.startStateQuery();
}

Dialog::~Dialog() { delete ui; }

void Dialog::onControlStateChanged(DeviceStatus oldState,
                                   DeviceStatus newState) {

  QString newStateStr =
      QString::fromStdString(Devices::Device::deviceStatusToString(newState));
  this->ui->tbl_control_workers->setItem(0, 0,
                                         new QTableWidgetItem(newStateStr));
}

void Dialog::onControlSubStateChanged(ControlWorkerSubState oldState,
                                      ControlWorkerSubState newState) {
  QString newStateStr =
      ControlWorkerWrapper::controlWorkerSubStateToString(newState);
  this->ui->tbl_control_workers->setItem(0, 1,
                                         new QTableWidgetItem(newStateStr));
}

void Dialog::onRemoteStatesChanged(
    QList<std::shared_ptr<StatusPayload>> oldStates,
    QList<std::shared_ptr<StatusPayload>> newStates) {

  this->ui->tbl_remote_workers->setRowCount(newStates.count() + 1);
  this->ui->tbl_remote_workers->setColumnCount(3);

  int row = 0;
  for (auto newState : newStates) {
    QString deviceName = QString::fromStdString(newState->getDeviceName());
    QString state = QString::fromStdString(
        Device::deviceStatusToString(newState->getDeviceStatus()));
    int deviceType = static_cast<int>(newState->getDeviceType());

    this->ui->tbl_remote_workers->setItem(row, 0,
                                          new QTableWidgetItem(deviceName));
    this->ui->tbl_remote_workers->setItem(row, 1, new QTableWidgetItem(state));
    this->ui->tbl_remote_workers->setItem(row, 2,
                                          new QTableWidgetItem(deviceType));
    row++;
  }
}
