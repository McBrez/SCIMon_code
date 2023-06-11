// Qt includes
#include "dialog.h"

// UI includes
#include "ui_dialog.h"

// Project includes
#include <device.hpp>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    controlWorkerWrapper(100, parent)
{
    ui->setupUi(this);

    this->ui->tbl_control_workers->setRowCount(1);
    this->ui->tbl_control_workers->setColumnCount(1);

    QObject::connect(
        this,
        &Dialog::finished,
        &this->controlWorkerWrapper,
        &ControlWorkerWrapper::shutdown);

    QObject::connect(
        this->ui->btn_connect,
        &QPushButton::clicked,
        &this->controlWorkerWrapper,
        [this](){
            this->controlWorkerWrapper.startConnection(
                this->ui->txt_ip->text(),
                this->ui->txt_port->text().toInt());
        });

    QObject::connect(
        &this->controlWorkerWrapper,
        &ControlWorkerWrapper::stateChanged,
        this,
        &Dialog::onControlStateChanged,
        Qt::QueuedConnection);

    QObject::connect(
        &this->controlWorkerWrapper,
        &ControlWorkerWrapper::subStateChanged,
        this,
        &Dialog::onControlSubStateChanged,
        Qt::QueuedConnection);

    this->controlWorkerWrapper.start();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::onControlStateChanged(DeviceStatus oldState, DeviceStatus newState) {

    QString newStateStr = QString::fromStdString(Devices::Device::deviceStatusToString(newState));
    this->ui->tbl_control_workers->setItem(0,0,new QTableWidgetItem(newStateStr));
}

void Dialog::onControlSubStateChanged(ControlWorkerSubState oldState,
                                      ControlWorkerSubState newState) {
    QString newStateStr = ControlWorkerWrapper::controlWorkerSubStateToString(newState);
    this->ui->tbl_control_workers->setItem(0,1,new QTableWidgetItem(newStateStr));
}
