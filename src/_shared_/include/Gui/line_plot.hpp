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
 * @brief A line plot that allows to display multiple time series data.
 */
class LinePlot : public QwtPlot {
  Q_OBJECT

public:
  LinePlot(QWidget *parent = Q_NULLPTR);

public slots:

  void pushData(const QString &title,
                const std::vector<std::tuple<TimePoint, double>> &data);
  void resetData();
  bool createCurve(const QString &title, QPen pen);
  bool removeCurve(const QString &title);
  QStringList getCurveTitle() const;
private:
  QList<QwtPlotCurve *> curves;
  QMap<QString, QPolygonF *> curveData;
};
} // namespace Gui
#endif
