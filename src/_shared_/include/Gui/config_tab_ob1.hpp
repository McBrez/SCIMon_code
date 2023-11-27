#ifndef CONFIG_TAB_OB1_HPP
#define CONFIG_TAB_OB1_HPP

// 3rd party includes
#include <QLineEdit>

// Project includes
#include <config_tab.hpp>

namespace Gui {

class ConfigTabOb1 : public ConfigTab {
public:
  ConfigTabOb1(QWidget *parent = Q_NULLPTR);

  virtual std::shared_ptr<InitPayload> getInitPayload() override;

  virtual std::shared_ptr<ConfigurationPayload> getConfigPayload() override;

  virtual QString configTabName() const override;

private:
  int strToChannelConfig(const QString &chnnelConfig);

  QLineEdit *txtDeviceName;
  QComboBox *cmbCh1;
  QComboBox *cmbCh2;
  QComboBox *cmbCh3;
  QComboBox *cmbCh4;
};

} // namespace Gui

#endif
