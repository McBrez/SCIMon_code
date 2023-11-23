// 3rd party includes
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

// Project includes
#include <config_tab_isx3.hpp>

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
  QComboBox *cmbComPort = new QComboBox(this);
  cmbComPort->addItems(QStringList() << "COM1"
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
  QSpinBox *spnStartFreq = new QSpinBox(this);
  spnStartFreq->setMinimum(0);
  spnStartFreq->setMaximum(20000000);
  spnStartFreq->setSuffix("Hz");
  hLayout2->addWidget(spnStartFreq);

  // Stopping Frequency
  QHBoxLayout *hLayout3 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout3);
  QLabel *labelStopFreq = new QLabel(this);
  labelStopFreq->setText("Stopping Frequency");
  hLayout3->addWidget(labelStopFreq);
  QSpinBox *spnStopFreq = new QSpinBox(this);
  spnStopFreq->setMinimum(0);
  spnStopFreq->setMaximum(20000000);
  spnStopFreq->setSuffix("Hz");
  hLayout3->addWidget(spnStopFreq);

  // Frequency Points
  QHBoxLayout *hLayout6 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout6);
  QLabel *labelFreqPoints = new QLabel(this);
  labelFreqPoints->setText("Frequency Points");
  hLayout6->addWidget(labelFreqPoints);
  QSpinBox *spnFreqPoints = new QSpinBox(this);
  spnFreqPoints->setMinimum(0);
  spnFreqPoints->setMaximum(100000);
  hLayout6->addWidget(spnFreqPoints);

  // Scale
  QHBoxLayout *hLayout8 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout8);
  QLabel *labelScale = new QLabel(this);
  labelScale->setText("Scale");
  hLayout8->addWidget(labelScale);
  QComboBox *cmbScale = new QComboBox(this);
  cmbScale->addItems(QStringList() << "Linear"
                                   << "Logarithmic");
  hLayout8->addWidget(cmbScale);

  // Repetitions
  QHBoxLayout *hLayout7 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout7);
  QLabel *labelRepetitions = new QLabel(this);
  labelRepetitions->setText("Repetitions");
  hLayout7->addWidget(labelRepetitions);
  QSpinBox *spnRepetitions = new QSpinBox(this);
  spnRepetitions->setMinimum(0);
  spnRepetitions->setMaximum(100000);
  hLayout7->addWidget(spnRepetitions);

  // Precision
  QHBoxLayout *hLayout4 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout4);
  QLabel *labelPrecision = new QLabel(this);
  labelPrecision->setText("Precision");
  hLayout4->addWidget(labelPrecision);
  QDoubleSpinBox *spnPrecision = new QDoubleSpinBox(this);
  spnPrecision->setMinimum(0.0);
  spnPrecision->setMaximum(5.0);
  hLayout4->addWidget(spnPrecision);

  // Amplitude
  QHBoxLayout *hLayout5 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout5);
  QLabel *labelAmplitude = new QLabel(this);
  labelAmplitude->setText("Amplitude");
  hLayout5->addWidget(labelAmplitude);
  QDoubleSpinBox *spnAmplitude = new QDoubleSpinBox(this);
  spnAmplitude->setMinimum(0.0);
  spnAmplitude->setMaximum(5.0);
  hLayout5->addWidget(spnAmplitude);
}

std::shared_ptr<InitPayload> ConfigTabIsx3::getInitPayload() {
  return std::shared_ptr<InitPayload>();
}

std::shared_ptr<ConfigurationPayload> ConfigTabIsx3::getConfigPayload() {
  return std::shared_ptr<ConfigurationPayload>();
}

QString ConfigTabIsx3::configTabName() const { return "Isx3"; }
