#ifndef DIALOG_H
#define DIALOG_H

// Qt includes
#include <QDialog>

// Project inclues
#include <config_tab.hpp>
#include <control_worker_wrapper.hpp>
#include <controller_plot.hpp>
#include <spectroplot.hpp>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog {
  Q_OBJECT

public:
  explicit Dialog(QWidget *parent = nullptr);
  ~Dialog();

public slots:
  void onControlStateChanged(DeviceStatus oldState, DeviceStatus newState);
  void onControlSubStateChanged(ControlWorkerSubState oldState,
                                ControlWorkerSubState newState);
  void onRemoteStatesChanged(QList<std::shared_ptr<StatusPayload>> oldStates,
                             QList<std::shared_ptr<StatusPayload>> newStates);
  void handleNewPressureData(
      const std::map<std::string, std::vector<std::tuple<TimePoint, double>>>
          &data);
  void onSetPressure();
  void startConfig();

private:
  Ui::Dialog *ui;
  ControlWorkerWrapper controlWorkerWrapper;
  Gui::Spectroplot *spectroplot;
  Gui::ControllerPlot *linePlot;
  QList<Gui::ConfigTab *> configTabs;
};

#endif // DIALOG_H
