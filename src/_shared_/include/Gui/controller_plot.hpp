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

public:
  ControllerPlot(QWidget *parent = Q_NULLPTR);

public slots:

  /**
   * @brief createCurve
   * @param title
   * @param isSetPoint
   * @return
   */
  bool createCurvePair(const QString &setPointTitle,
                       const QString &curValueTitle);
};
} // namespace Gui
#endif
