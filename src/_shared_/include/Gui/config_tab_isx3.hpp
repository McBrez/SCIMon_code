#ifndef CONFIG_TAB_ISX3_HPP
#define CONFIG_TAB_ISX3_HPP

// 3rd party inlcudes
// Qt
#include <QComboBox>
#include <QDoubleSpinBox>

// Project includes
#include <config_tab.hpp>

using namespace Devices;

namespace Gui {

class ConfigTabIsx3 : public ConfigTab {
public:
  ConfigTabIsx3(QWidget *parent = Q_NULLPTR);

  virtual std::shared_ptr<InitPayload> getInitPayload() override;

  virtual std::shared_ptr<ConfigurationPayload> getConfigPayload() override;

  virtual QString configTabName() const override;

private:
  QComboBox *cmbComPort;
  QSpinBox *spnStartFreq;
  QSpinBox *spnStopFreq;
  QSpinBox *spnFreqPoints;
  QComboBox *cmbScale;
  QSpinBox *spnRepetitions;
  QDoubleSpinBox *spnPrecision;
  QDoubleSpinBox *spnAmplitude;
  QComboBox *cmbMeasConf;
  QSpinBox *spnCp;
  QSpinBox *spnRp;
  QSpinBox *spnWs;
  QSpinBox *spnWp;
  QComboBox *cmbMeasRange;
};

} // namespace Gui

#endif
