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
#include <config_tab_sentry.hpp>
#include <sentry_config_payload.hpp>

using namespace Gui;

ConfigTabSentry::ConfigTabSentry(QWidget *parent) : ConfigTab(parent) {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // Init label
  QLabel *labelInit = new QLabel(this);
  labelInit->setText("<b>Configuration Data</b>");
  mainLayout->addWidget(labelInit);

  // Device Name
  QHBoxLayout *hLayout1 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout1);
  QLabel *labelOperationMode = new QLabel(this);
  labelOperationMode->setText("Operation Mode");
  hLayout1->addWidget(labelOperationMode);
  this->cmbOperationMode = new QComboBox(this);
  this->cmbOperationMode->addItems(QStringList() << "Manual"
                                                 << "Timer");
  hLayout1->addWidget(this->cmbOperationMode);

  // On time
  QHBoxLayout *hLayout2 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout2);
  QLabel *labelOnTime = new QLabel(this);
  labelOnTime->setText("On Time");
  hLayout2->addWidget(labelOnTime);
  this->spnOnTime = new QSpinBox(this);
  this->spnOnTime->setMinimum(0);
  this->spnOnTime->setValue(5);
  this->spnOnTime->setSuffix("s");
  hLayout2->addWidget(this->spnOnTime);

  // Off time
  QHBoxLayout *hLayout3 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout3);
  QLabel *labelOffTime = new QLabel(this);
  labelOffTime->setText("Off Time");
  hLayout3->addWidget(labelOffTime);
  this->spnOffTime = new QSpinBox(this);
  this->spnOffTime->setMinimum(0);
  this->spnOffTime->setValue(5);
  this->spnOffTime->setSuffix("s");
  hLayout3->addWidget(this->spnOffTime);

  QSpacerItem *spacer =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  mainLayout->addItem(spacer);

  QObject::connect(this->cmbOperationMode, &QComboBox::currentTextChanged, this,
                   &ConfigTabSentry::onWorkerModeChanged);

  this->onWorkerModeChanged(this->cmbOperationMode->currentText());
}

std::shared_ptr<InitPayload> ConfigTabSentry::getInitPayload() {
  return std::shared_ptr<InitPayload>();
}

std::shared_ptr<ConfigurationPayload> ConfigTabSentry::getConfigPayload() {

  Workers::SentryWorkerMode workerMode =
      this->strToWorkerMode(this->cmbOperationMode->currentText());
  Duration onTime(std::chrono::seconds(this->spnOnTime->value()));
  Duration offTime(std::chrono::seconds(this->spnOffTime->value()));

  return std::shared_ptr<ConfigurationPayload>(
      new Workers::SentryConfigPayload(workerMode, onTime, offTime));
}

Workers::SentryWorkerMode
ConfigTabSentry::strToWorkerMode(const QString &workerModeStr) {
  if (workerModeStr == "Manual") {
    return Workers::SentryWorkerMode::SENTRY_WORKER_MODE_MANUAL;
  } else if (workerModeStr == "Timer") {
    return Workers::SentryWorkerMode::SENTRY_WORKER_MODE_TIMER;
  } else {
    return Workers::SentryWorkerMode::SENTRY_WORKER_MODE_INVALID;
  }
}

QString ConfigTabSentry::configTabName() const { return "Sentry"; }

void ConfigTabSentry::onWorkerModeChanged(const QString &text) {
  Workers::SentryWorkerMode workerMode = this->strToWorkerMode(text);

  if (Workers::SentryWorkerMode::SENTRY_WORKER_MODE_MANUAL == workerMode) {
    this->spnOffTime->setEnabled(false);
    this->spnOnTime->setEnabled(false);
  } else {
    this->spnOffTime->setEnabled(true);
    this->spnOnTime->setEnabled(true);
  }
}
