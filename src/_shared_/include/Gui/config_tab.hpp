#ifndef CONFIG_TAB_HPP
#define CONFIG_TAB_HPP

// 3rd party includes
// Qt
#include <QTabWidget>

// Project includes
#include <configuration_payload.hpp>
#include <init_payload.hpp>

using namespace Devices;

namespace Gui {

class ConfigTab : public QTabWidget {
public:
  ConfigTab(QWidget *parent = Q_NULLPTR);

  virtual std::shared_ptr<InitPayload> getInitPayload() = 0;

  virtual std::shared_ptr<ConfigurationPayload> getConfigPayload() = 0;

  virtual QString configTabName() const = 0;
};

} // namespace Gui

#endif
