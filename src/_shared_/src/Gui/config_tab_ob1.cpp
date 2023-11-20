// Project includes
#include <config_tab_ob1.hpp>

using namespace Gui;

ConfigTabOb1::ConfigTabOb1(QWidget *parent) : ConfigTab(parent) {}

std::shared_ptr<InitPayload> ConfigTabOb1::getInitPayload() {
  return std::shared_ptr<InitPayload>();
}

std::shared_ptr<ConfigurationPayload> ConfigTabOb1::getConfigPayload() {
  return std::shared_ptr<ConfigurationPayload>();
}
