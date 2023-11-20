#ifndef CONFIG_TAB_OB1_HPP
#define CONFIG_TAB_OB1_HPP

// Project includes
#include <config_tab.hpp>

namespace Gui {

class ConfigTabOb1 : public ConfigTab {
public:
  ConfigTabOb1(QWidget *parent = Q_NULLPTR);

  virtual std::shared_ptr<InitPayload> getInitPayload() override;

  virtual std::shared_ptr<ConfigurationPayload> getConfigPayload() override;
};

} // namespace Gui

#endif
