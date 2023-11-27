// 3rd party includes
// Qt
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
// Elveflow
#include <Elveflow64_shim.h>

// Project includes
#include <config_tab_ob1.hpp>
#include <ob1_init_payload.hpp>

using namespace Gui;

ConfigTabOb1::ConfigTabOb1(QWidget *parent) : ConfigTab(parent) {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // Init label
  QLabel *labelInit = new QLabel(this);
  labelInit->setText("<b>Initialization Data</b>");
  mainLayout->addWidget(labelInit);

  // Device Name
  QHBoxLayout *hLayout1 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout1);
  QLabel *labelComPort = new QLabel(this);
  labelComPort->setText("VISA Device Name");
  hLayout1->addWidget(labelComPort);
  this->txtDeviceName = new QLineEdit(this);
  hLayout1->addWidget(txtDeviceName);

  // Channel type
  QLabel *labelChannelType = new QLabel(this);
  labelChannelType->setText("Channel Type");
  mainLayout->addWidget(labelChannelType);

  QHBoxLayout *hLayout2 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout2);
  QLabel *labelCh1 = new QLabel(this);
  labelCh1->setText("Ch1");
  hLayout2->addWidget(labelCh1);
  this->cmbCh1 = new QComboBox(this);
  this->cmbCh1->addItems(QStringList() << "0-200mBar"
                                       << "0-2000mBar"
                                       << "0-8000mBar"
                                       << "-1000-1000mBar");
  hLayout2->addWidget(this->cmbCh1);
  QLabel *labelCh2 = new QLabel(this);
  labelCh2->setText("Ch2");
  hLayout2->addWidget(labelCh2);
  this->cmbCh2 = new QComboBox(this);
  this->cmbCh2->addItems(QStringList() << "0-200mBar"
                                       << "0-2000mBar"
                                       << "0-8000mBar"
                                       << "-1000-1000mBar");
  hLayout2->addWidget(this->cmbCh2);
  QLabel *labelCh3 = new QLabel(this);
  labelCh3->setText("Ch3");
  hLayout2->addWidget(labelCh3);
  this->cmbCh3 = new QComboBox(this);
  this->cmbCh3->addItems(QStringList() << "0-200mBar"
                                       << "0-2000mBar"
                                       << "0-8000mBar"
                                       << "-1000-1000mBar");
  hLayout2->addWidget(this->cmbCh3);
  QLabel *labelCh4 = new QLabel(this);
  labelCh4->setText("Ch4");
  hLayout2->addWidget(labelCh4);
  this->cmbCh4 = new QComboBox(this);
  this->cmbCh4->addItems(QStringList() << "0-200mBar"
                                       << "0-2000mBar"
                                       << "0-8000mBar"
                                       << "-1000-1000mBar");
  hLayout2->addWidget(this->cmbCh4);

  // Configuration label
  QLabel *labelConfig = new QLabel(this);
  labelConfig->setText("<b>Configuration Data</b>");
  mainLayout->addWidget(labelConfig);
  // Preset pressures
  QLabel *labelPresetPressure = new QLabel(this);
  labelPresetPressure->setText("Preset Pressures");
  mainLayout->addWidget(labelPresetPressure);

  QHBoxLayout *hLayout3 = new QHBoxLayout(this);

  mainLayout->addLayout(hLayout3);
  QLabel *labelPressCh1 = new QLabel(this);
  labelPressCh1->setText("Ch1");
  hLayout3->addWidget(labelPressCh1);
  this->cmbPressCh1 = new QDoubleSpinBox(this);
  hLayout3->addWidget(this->cmbPressCh1);

  mainLayout->addLayout(hLayout3);
  QLabel *labelPressCh2 = new QLabel(this);
  labelPressCh2->setText("Ch2");
  hLayout3->addWidget(labelPressCh2);
  this->cmbPressCh2 = new QDoubleSpinBox(this);
  hLayout3->addWidget(this->cmbPressCh2);

  mainLayout->addLayout(hLayout3);
  QLabel *labelPressCh3 = new QLabel(this);
  labelPressCh3->setText("Ch3");
  hLayout3->addWidget(labelPressCh3);
  this->cmbPressCh3 = new QDoubleSpinBox(this);
  hLayout3->addWidget(this->cmbPressCh3);

  mainLayout->addLayout(hLayout3);
  QLabel *labelPressCh4 = new QLabel(this);
  labelPressCh4->setText("Ch4");
  hLayout3->addWidget(labelPressCh4);
  this->cmbPressCh4 = new QDoubleSpinBox(this);
  hLayout3->addWidget(this->cmbPressCh4);

  QSpacerItem *spacer =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  mainLayout->addItem(spacer);
}

std::shared_ptr<InitPayload> ConfigTabOb1::getInitPayload() {
  std::string deviceName = this->txtDeviceName->text().toStdString();
  ChannelConfiguration channelConf =
      std::make_tuple(this->strToChannelConfig(this->cmbCh1->currentText()),
                      this->strToChannelConfig(this->cmbCh2->currentText()),
                      this->strToChannelConfig(this->cmbCh3->currentText()),
                      this->strToChannelConfig(this->cmbCh4->currentText()));

  return std::shared_ptr<InitPayload>(
      new Ob1InitPayload(deviceName, channelConf));
}

std::shared_ptr<ConfigurationPayload> ConfigTabOb1::getConfigPayload() {
  return std::shared_ptr<ConfigurationPayload>();
}

QString ConfigTabOb1::configTabName() const { return "Ob1"; }

int ConfigTabOb1::strToChannelConfig(const QString &chnnelConfig) {

  if (chnnelConfig == "0-200mBar") {
    return Z_regulator_type__0_200_mbar;
  } else if (chnnelConfig == "0-2000mBar") {
    return Z_regulator_type__0_2000_mbar;
  } else if (chnnelConfig == "0-8000mBar") {
    return Z_regulator_type__0_8000_mbar;
  } else if (chnnelConfig == "-1000-1000mBar") {
    return Z_regulator_type_m1000_1000_mbar;
  } else {
    return Z_regulator_type_none;
  }
}
