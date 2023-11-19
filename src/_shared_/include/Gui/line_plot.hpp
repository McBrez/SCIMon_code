#ifndef LINE_PLOT_HPP
#define LINE_PLOT_HPP

// 3rd party include
// Qt
#include <QPen>
// Qwt
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

// Project includes
#include <common.hpp>
#include <data_manager.hpp>

namespace Gui {
/**
 * @brief A line plot that allows to display multiple time series data rows.
 */
class LinePlot : public QwtPlot {
  Q_OBJECT

public:
  /**
   * @brief Creates the object.
   * @param parent The Qt parent.
   * @param duration Data that is older than this duration, will be removed from
   * the plot.
   */
  LinePlot(QWidget *parent = Q_NULLPTR,
           Core::Duration duration = Core::Duration());

public slots:

  void pushData(const QString &title,
                const std::vector<std::tuple<TimePoint, double>> &data);
  void resetData();
  bool createCurve(const QString &title, QPen pen);
  bool removeCurve(const QString &title);
  QStringList getCurveTitles() const;

private:
  QList<QwtPlotCurve *> curves;
  QMap<QString, QPolygonF *> curveData;
  Core::Duration duration;
};
} // namespace Gui
#endif
