// 3rd party includes
// Qt
#include <QSet>
// Qwt
#include <qwt_scale_widget.h>

// UI includes
#include "ui_dialog.h"

// Project includes
#include <config_tab_isx3.hpp>
#include <config_tab_ob1.hpp>
#include <device.hpp>
#include <dialog.h>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog), controlWorkerWrapper(100, parent),
      spectroplot(new Gui::Spectroplot(std::vector<double>{0.0},
                                       std::chrono::minutes(1), this)),
      linePlot(new Gui::ControllerPlot("currPressure", "setpoint",
                                       std::chrono::minutes(1), this)) {

  ui->setupUi(this);

  // Add the spectroplot.
  this->spectroplot->setContentsMargins(0, 5, 0, 10);
  this->spectroplot->resize(400, 220);

  // Add the line plot.
  this->linePlot->resize(450, 220);
  this->linePlot->move(450, 0);
  this->linePlot->enableAxis(QwtAxis::YLeft, false);
  this->linePlot->enableAxis(QwtAxis::YRight, true);
  this->linePlot->axisWidget(QwtAxis::YRight)->setTitle("Pressure (bar)");

  // Add the state table.
  this->ui->tbl_control_workers->setRowCount(1);
  this->ui->tbl_control_workers->setColumnCount(2);

  ui->cmb_channel_selection->addItems(QStringList() << "Ch1"
                                                    << "Ch2"
                                                    << "Ch3"
                                                    << "Ch4");

  // Set up the configuration tabs.
  this->ui->tab_config->addTab(new Gui::ConfigTabIsx3(), "Isx3");
  this->ui->tab_config->addTab(new Gui::ConfigTabOb1(), "Ob1");

  QObject::connect(this->ui->btn_set_pressure, &QPushButton::clicked, this,
                   &Dialog::onSetPressure);

  QObject::connect(this, &Dialog::finished, &this->controlWorkerWrapper,
                   &ControlWorkerWrapper::shutdown);

  QObject::connect(this->ui->btn_connect, &QPushButton::clicked,
                   &this->controlWorkerWrapper, [this]() {
                     this->controlWorkerWrapper.startConnection(
                         this->ui->txt_ip->text(),
                         this->ui->txt_port->text().toInt());
                   });
  QObject::connect(this->ui->btn_configControl, &QPushButton::clicked,
                   &this->controlWorkerWrapper, [this]() {
                     this->controlWorkerWrapper.startConfig(
                         this->ui->txt_isxComPort->text(),
                         this->ui->txt_ob1DeviceName->text());
                   });

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
  QObject::connect(this->ui->btn_start_isx3, &QPushButton::clicked,
                   &this->controlWorkerWrapper,
                   &ControlWorkerWrapper::start_isx3);
  QObject::connect(this->ui->btn_stop_isx3, &QPushButton::clicked,
                   &this->controlWorkerWrapper,
                   &ControlWorkerWrapper::stop_isx3);
  QObject::connect(this->ui->btn_start_ob1, &QPushButton::clicked,
                   &this->controlWorkerWrapper,
                   &ControlWorkerWrapper::start_ob1);
  QObject::connect(this->ui->btn_stop_ob1, &QPushButton::clicked,
                   &this->controlWorkerWrapper,
                   &ControlWorkerWrapper::stop_ob1);

  QObject::connect(
      &this->controlWorkerWrapper, &ControlWorkerWrapper::newSpectrumData,
      this->spectroplot,
      [this](
          const std::vector<std::tuple<TimePoint, ImpedanceSpectrum>> &data) {
        for (auto tup : data) {
          this->spectroplot->pushSpectrum(std::get<TimePoint>(tup),
                                          std::get<ImpedanceSpectrum>(tup));
        }
      });
  QObject::connect(&this->controlWorkerWrapper,
                   &ControlWorkerWrapper::newPressureData, this,
                   &Dialog::handleNewPressureData);

  this->controlWorkerWrapper.startStateQuery();
}

Dialog::~Dialog() { delete ui; }

void Dialog::handleNewPressureData(
    const std::map<std::string, std::vector<std::tuple<TimePoint, double>>>
        &data) {

  // Define a lamba, that strips user id and timestamp from the data key.
  auto strip = [](std::string str) {
    QString qStr = QString::fromStdString(str);
    QStringList strSplit = qStr.split("/");
    QStringList strListStripped(strSplit.begin() + 2, strSplit.end());

    return strListStripped.join("/");
  };

  // Check if there are new curves ...
  // ... First build string lists that contain the curve title ...
  QStringList knownCurveTitles = this->linePlot->getCurveTitles();
  QStringList dataCurveTitles;
  for (auto &keyValuePair : data) {
    // ... Remove the user id from the tag ...
    dataCurveTitles << strip(keyValuePair.first);
  }

  // Curves that are in dataCurveTitles, but not in knownCurveTitles are new.
  QSet<QString> knownCurveTitlesSet(knownCurveTitles.begin(),
                                    knownCurveTitles.end());
  QSet<QString> dataCurveTitlesSet(dataCurveTitles.begin(),
                                   dataCurveTitles.end());
  QSet<QString> newCurveTitles = dataCurveTitlesSet - knownCurveTitlesSet;

  // Add new curves.
  if (!newCurveTitles.empty()) {
    for (auto &newCurveTitle : newCurveTitles) {
      this->linePlot->createCurvePair(newCurveTitle);
    }
  }

  // Push data.
  for (auto &keyValuePair : data) {
    this->linePlot->pushData(strip(keyValuePair.first), keyValuePair.second);
  }
}

void Dialog::onControlSubStateChanged(ControlWorkerSubState oldState,
                                      ControlWorkerSubState newState) {
  QString newStateStr =
      ControlWorkerWrapper::controlWorkerSubStateToString(newState);
  this->ui->tbl_control_workers->setItem(0, 1,
                                         new QTableWidgetItem(newStateStr));
}

void Dialog::onControlStateChanged(DeviceStatus oldState,
                                   DeviceStatus newState) {

  QString newStateStr =
      QString::fromStdString(Devices::Device::deviceStatusToString(newState));
  this->ui->tbl_control_workers->setItem(0, 0,
                                         new QTableWidgetItem(newStateStr));
}

void Dialog::onRemoteStatesChanged(
    QList<std::shared_ptr<StatusPayload>> oldStates,
    QList<std::shared_ptr<StatusPayload>> newStates) {
  this->ui->tbl_remote_workers->setRowCount(newStates.count() + 1);
  this->ui->tbl_remote_workers->setColumnCount(3);

  int row = 0;
  for (auto newState : newStates) {
    // Update the table.
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

void Dialog::onSetPressure() {
  int channel = this->ui->cmb_channel_selection->currentIndex() + 1;
  double pressure = this->ui->spn_pressure->value();

  this->controlWorkerWrapper.setPressure(channel, pressure);
}
