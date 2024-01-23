// 3rd party includes
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

// Project includes
#include <config_tab_isx3.hpp>
#include <is_configuration.hpp>
#include <isx3_constants.hpp>
#include <isx3_init_payload.hpp>
#include <isx3_is_conf_payload.hpp>

using namespace Devices;
using namespace Gui;

ConfigTabIsx3::ConfigTabIsx3(QWidget *parent) : ConfigTab(parent) {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // Init label
  QLabel *labelInit = new QLabel(this);
  labelInit->setText("<b>Initialization Data</b>");
  mainLayout->addWidget(labelInit);
  // COM Port
  QHBoxLayout *hLayout1 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout1);
  QLabel *labelComPort = new QLabel(this);
  labelComPort->setText("COM Port");
  hLayout1->addWidget(labelComPort);
  this->cmbComPort = new QComboBox(this);
  this->cmbComPort->addItems(QStringList() << "COM1"
                                           << "COM2"
                                           << "COM3"
                                           << "COM4"
                                           << "COM5"
                                           << "COM6"
                                           << "COM7"
                                           << "COM8"
                                           << "COM9"
                                           << "COM10"
                                           << "COM11"
                                           << "COM12"
                                           << "COM13"
                                           << "COM14"
                                           << "COM15"
                                           << "COM16"
                                           << "COM17"
                                           << "COM18"
                                           << "COM19");
  hLayout1->addWidget(cmbComPort);

  // Config Label
  QLabel *labelConfig = new QLabel(this);
  labelConfig->setText("<b>Configuration Data</b>");
  mainLayout->addWidget(labelConfig);

  // Starting Frequency
  QHBoxLayout *hLayout2 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout2);
  QLabel *labelStartFreq = new QLabel(this);
  labelStartFreq->setText("Starting Frequency");
  hLayout2->addWidget(labelStartFreq);
  this->spnStartFreq = new QSpinBox(this);
  spnStartFreq->setMinimum(0);
  spnStartFreq->setMaximum(20000000);
  spnStartFreq->setValue(1);
  spnStartFreq->setSuffix("Hz");
  hLayout2->addWidget(spnStartFreq);

  // Stopping Frequency
  QHBoxLayout *hLayout3 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout3);
  QLabel *labelStopFreq = new QLabel(this);
  labelStopFreq->setText("Stopping Frequency");
  hLayout3->addWidget(labelStopFreq);
  this->spnStopFreq = new QSpinBox(this);
  spnStopFreq->setMinimum(0);
  spnStopFreq->setMaximum(20000000);
  spnStopFreq->setValue(1000);
  spnStopFreq->setSuffix("Hz");
  hLayout3->addWidget(spnStopFreq);

  // Frequency Points
  QHBoxLayout *hLayout6 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout6);
  QLabel *labelFreqPoints = new QLabel(this);
  labelFreqPoints->setText("Frequency Points");
  hLayout6->addWidget(labelFreqPoints);
  this->spnFreqPoints = new QSpinBox(this);
  spnFreqPoints->setMinimum(0);
  spnFreqPoints->setMaximum(100000);
  spnFreqPoints->setValue(10);
  hLayout6->addWidget(spnFreqPoints);

  // Scale
  QHBoxLayout *hLayout8 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout8);
  QLabel *labelScale = new QLabel(this);
  labelScale->setText("Scale");
  hLayout8->addWidget(labelScale);
  this->cmbScale = new QComboBox(this);
  cmbScale->addItems(QStringList() << "Linear"
                                   << "Logarithmic");
  hLayout8->addWidget(cmbScale);

  // Repetitions
  QHBoxLayout *hLayout7 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout7);
  QLabel *labelRepetitions = new QLabel(this);
  labelRepetitions->setText("Repetitions");
  hLayout7->addWidget(labelRepetitions);
  this->spnRepetitions = new QSpinBox(this);
  spnRepetitions->setMinimum(0);
  spnRepetitions->setMaximum(100000);
  spnRepetitions->setValue(0);
  hLayout7->addWidget(spnRepetitions);

  // Precision
  QHBoxLayout *hLayout4 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout4);
  QLabel *labelPrecision = new QLabel(this);
  labelPrecision->setText("Precision");
  hLayout4->addWidget(labelPrecision);
  this->spnPrecision = new QDoubleSpinBox(this);
  spnPrecision->setMinimum(0.0);
  spnPrecision->setMaximum(5.0);
  spnPrecision->setValue(1.0);
  hLayout4->addWidget(spnPrecision);

  // Amplitude
  QHBoxLayout *hLayout5 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout5);
  QLabel *labelAmplitude = new QLabel(this);
  labelAmplitude->setText("Amplitude");
  hLayout5->addWidget(labelAmplitude);
  this->spnAmplitude = new QDoubleSpinBox(this);
  spnAmplitude->setMinimum(0.0);
  spnAmplitude->setMaximum(5.0);
  spnAmplitude->setValue(1.0);
  hLayout5->addWidget(spnAmplitude);

  // Measurement configuration
  QHBoxLayout *hLayout10 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout10);
  QLabel *labelMeasConf = new QLabel(this);
  labelMeasConf->setText("Measurement Conf");
  hLayout10->addWidget(labelMeasConf);
  this->cmbMeasConf = new QComboBox(this);
  cmbMeasConf->addItems(QStringList() << "2 Point"
                                      << "3 Point"
                                      << "4 Point");
  hLayout10->addWidget(cmbMeasConf);

  // Pin numbers
  QHBoxLayout *hLayout9 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout9);
  QLabel *labelCp = new QLabel(this);
  labelCp->setText("CP");
  hLayout9->addWidget(labelCp);
  this->spnCp = new QSpinBox(this);
  spnCp->setMinimum(0);
  spnCp->setMaximum(64);
  hLayout9->addWidget(spnCp);
  QLabel *labelRp = new QLabel(this);
  labelRp->setText("RP");
  hLayout9->addWidget(labelRp);
  this->spnRp = new QSpinBox(this);
  spnRp->setMinimum(0);
  spnRp->setMaximum(64);
  hLayout9->addWidget(spnRp);
  QLabel *labelWs = new QLabel(this);
  labelWs->setText("WS");
  hLayout9->addWidget(labelWs);
  this->spnWs = new QSpinBox(this);
  spnWs->setMinimum(0);
  spnWs->setMaximum(64);
  hLayout9->addWidget(spnWs);
  QLabel *labelWp = new QLabel(this);
  labelWp->setText("WP");
  hLayout9->addWidget(labelWp);
  this->spnWp = new QSpinBox(this);
  spnWp->setMinimum(0);
  spnWp->setMaximum(64);
  hLayout9->addWidget(spnWp);

  // Config range
  QHBoxLayout *hLayout11 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout11);
  QLabel *labelConfigRange = new QLabel(this);
  labelConfigRange->setText("Measurement Range");
  hLayout11->addWidget(labelConfigRange);
  this->cmbMeasRange = new QComboBox(this);
  cmbMeasRange->addItems(QStringList() << "10 mA"
                                       << "100 µA"
                                       << "1 µA"
                                       << "10 nA");
  hLayout11->addWidget(cmbMeasRange);
}

std::shared_ptr<InitPayload> ConfigTabIsx3::getInitPayload() {
  return std::shared_ptr<InitPayload>(new Isx3InitPayload(
      this->cmbComPort->currentText().toStdString(), 256000));
}

std::shared_ptr<ConfigurationPayload> ConfigTabIsx3::getConfigPayload() {
  std::map<ChannelFunction, int> channelFunction;
  channelFunction[CHAN_FUNC_CP] = this->spnCp->value();
  channelFunction[CHAN_FUNC_RP] = this->spnRp->value();
  channelFunction[CHAN_FUNC_WS] = this->spnWs->value();
  channelFunction[CHAN_FUNC_WP] = this->spnWp->value();

  IsScale isScale = this->cmbScale->currentText() == "Linear"
                        ? LINEAR_SCALE
                        : LOGARITHMIC_SCALE;

  QString measRange = this->cmbMeasRange->currentText();
  MeasurmentConfigurationRange measurementConfRange;
  if (measRange == "10 mA") {
    measurementConfRange = MEAS_CONFIG_RANGE_10MA;
  } else if ("100 µA") {
    measurementConfRange = MEAS_CONFIG_RANGE_100UA;
  } else if ("1 µA") {
    measurementConfRange = MEAS_CONFIG_RANGE_1UA;
  } else if ("10 nA") {
    measurementConfRange = MEAS_CONFIG_RANGE_10NA;
  }

  return std::shared_ptr<ConfigurationPayload>(new Isx3IsConfPayload(
      this->spnStartFreq->value(), this->spnStopFreq->value(),
      this->spnFreqPoints->value(), this->spnRepetitions->value(),
      channelFunction, isScale, measurementConfRange,
      MEAS_CONFIG_CHANNEL_EXT_PORT, MEAS_CONFIG_2_POINT,
      this->spnPrecision->value(), this->spnAmplitude->value()));
}

QString ConfigTabIsx3::configTabName() const { return "Isx3"; }
