#ifndef CONFIG_TAB_SENTRY_HPP
#define CONFIG_TAB_SENTRY_HPP

// 3rd party includes
// Qt
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>

// Project includes
#include <config_tab.hpp>
#include <sentry_config_payload.hpp>

namespace Gui {

class ConfigTabSentry : public ConfigTab {
public:
  ConfigTabSentry(QWidget *parent = Q_NULLPTR);

  virtual std::shared_ptr<InitPayload> getInitPayload() override;

  virtual std::shared_ptr<ConfigurationPayload> getConfigPayload() override;

  virtual QString configTabName() const override;

private slots:
  void onWorkerModeChanged(const QString &text);

private:
  Workers::SentryWorkerMode strToWorkerMode(const QString &workerModeStr);

  QComboBox *cmbOperationMode;
  QSpinBox *spnOnTime;
  QSpinBox *spnOffTime;
};

} // namespace Gui

#endif
