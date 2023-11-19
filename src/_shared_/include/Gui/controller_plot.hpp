#ifndef CONTROLLER_PLOT_HPP
#define CONTROLLER_PLOT_HPP

// Project includes
#include <line_plot.hpp>

namespace Gui {
/**
 * @brief A plot that displays current and set point values.
 */
class ControllerPlot : public LinePlot {
  Q_OBJECT

  /**
   * @brief A plot indented for drawing data from a controller. I.e. It draws
   * multiple pairs of setpoint/current time series data.
   * @param currentPressureStr If a curve is added, whose name contains this
   * string, it is interpreted as a current data time series.
   * @param setPointPressureStr If a curve is added, whose name contains this
   * string, it is interpreted a set point time series data.
   * @param duration Data that is older than this, will be removed from the
   * plot.
   * @param parent The Qt parent.
   */
public:
  ControllerPlot(QString currentPressureStr, QString setPointPressureStr,
                 Core::Duration duration = Core::Duration(),
                 QWidget *parent = Q_NULLPTR);

public slots:

  /**
   * @brief Create a pair or setPoint/currValue curves.
   * @param setPointTitle The name of the set point curve
   * @param curValueTitle The name of the current value curve.
   * @return
   */
  bool createCurvePair(const QString &setPointTitle,
                       const QString &curValueTitle);

  /**
   * @brief Creates the a curve with the given title. Depending on the name, the
   * curve will be created as a set point or current value curve. Its partner
   * will be creates aswell.
   * @param curveTitle The title of the curve that shall be created.
   * @return
   */
  bool createCurvePair(const QString &curveTitle);

private:
  QString currentPressureStr;
  QString setPointPressureStr;
};
} // namespace Gui
#endif
