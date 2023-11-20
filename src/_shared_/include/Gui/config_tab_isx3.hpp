#ifndef CONFIG_TAB_ISX3_HPP
#define CONFIG_TAB_ISX3_HPP

// Project includes
#include <config_tab.hpp>

using namespace Devices;

namespace Gui {

class ConfigTabIsx3 : public ConfigTab {
public:
  ConfigTabIsx3(QWidget *parent = Q_NULLPTR);

  virtual std::shared_ptr<InitPayload> getInitPayload() override;

  virtual std::shared_ptr<ConfigurationPayload> getConfigPayload() override;
};

} // namespace Gui

#endif
