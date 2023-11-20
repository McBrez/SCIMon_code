// 3rd party includes
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

// Project includes
#include <config_tab_isx3.hpp>

using namespace Devices;
using namespace Gui;

ConfigTabIsx3::ConfigTabIsx3(QWidget *parent) : ConfigTab(parent) {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  QHBoxLayout *hLayout = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout);

  mainLayout->addWidget(new Line);

  QLabel *label = new QLabel(this);
  label->setText("123123");
  hLayout->addWidget(label);
  QLineEdit *lineEdit = new QLineEdit(this);
  hLayout->addWidget(lineEdit);

  QHBoxLayout *hLayout1 = new QHBoxLayout(this);
  mainLayout->addLayout(hLayout1);
  QLabel *label1 = new QLabel(this);
  label1->setText("123123");
  hLayout1->addWidget(label1);
  QLineEdit *lineEdit1 = new QLineEdit(this);
  hLayout1->addWidget(lineEdit1);
}

std::shared_ptr<InitPayload> ConfigTabIsx3::getInitPayload() {
  return std::shared_ptr<InitPayload>();
}

std::shared_ptr<ConfigurationPayload> ConfigTabIsx3::getConfigPayload() {
  return std::shared_ptr<ConfigurationPayload>();
}
