// 3rd party includes
#include <qwt_date_scale_draw.h>
#include <qwt_date_scale_engine.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>

// Project includes
#include <line_plot.hpp>

namespace Gui {

LinePlot::LinePlot(QWidget *parent, Core::Duration duration)
    : QwtPlot(parent), duration(duration) {
  // Set up axes.
  this->axisWidget(QwtAxis::XBottom)->setTitle("Time");

  QwtDateScaleDraw *scaleXDraw = new QwtDateScaleDraw();
  scaleXDraw->setDateFormat(QwtDate::IntervalType::Minute, "hh:mm:ss");
  scaleXDraw->setDateFormat(QwtDate::Millisecond, "hh:mm:ss");
  scaleXDraw->setDateFormat(QwtDate::Second, "hh:mm:ss");
  scaleXDraw->setDateFormat(QwtDate::Minute, "hh:mm:ss");
  scaleXDraw->setDateFormat(QwtDate::Hour, "hh:mm:ss");
  scaleXDraw->setDateFormat(QwtDate::Day, "hh:mm:ss");
  scaleXDraw->setDateFormat(QwtDate::Week, "hh:mm:ss");
  scaleXDraw->setDateFormat(QwtDate::Month, "hh:mm:ss");
  this->setAxisScaleDraw(QwtAxis::XBottom, scaleXDraw);

  QwtScaleDraw *scaleYDraw = new QwtScaleDraw();
  this->setAxisScaleDraw(QwtAxis::YRight, scaleYDraw);

  QwtDateScaleEngine *scaleEngine = new QwtDateScaleEngine();
  this->setAxisScaleEngine(QwtAxis::XBottom, scaleEngine);

  this->insertLegend(new QwtLegend());

  QwtPlotGrid *grid = new QwtPlotGrid();
  grid->attach(this);

  this->plotLayout()->setAlignCanvasToScales(true);
  this->setAxisAutoScale(QwtAxis::YRight, true);
  this->setAxisAutoScale(QwtAxis::XBottom, true);
}

void LinePlot::pushData(
    const QString &title,
    const std::vector<std::tuple<TimePoint, double>> &data) {

  // Try to find the curve.
  auto it = std::find_if(
      this->curves.begin(), this->curves.end(),
      [title](QwtPlotCurve *curve) { return curve->title() == title; });

  if (it == this->curves.end()) {
    // Curve does not exist. Do nothing.

    return;
  }

  // Build a polygon from the given data.
  QPolygonF points;
  for (auto &point : data) {
    size_t timestamp = std::get<TimePoint>(point).time_since_epoch().count();
    double value = std::get<double>(point);
    points << QPointF(timestamp, value);
  }

  // Remove old data.
  if (this->duration.count() != 0) {
    Core::TimePoint minTimestamp = Core::getNow() - this->duration;
    for (auto &curve : this->curveData) {
      Core::Duration durationTemp = this->duration;
      curve->removeIf([minTimestamp](QPointF point) {
        return static_cast<long long>(point.x()) <
               minTimestamp.time_since_epoch().count();
      });
    }
  }

  // Append the constructed polygon to the data.
  (*this->curveData[title]) << points;
  (*it)->setSamples((*this->curveData[title]));

  this->replot();
}

bool LinePlot::createCurve(const QString &title, QPen pen) {
  // Check if a curve with the given title already exists.
  auto it = std::find_if(
      this->curves.begin(), this->curves.end(),
      [title](QwtPlotCurve *curve) { return curve->title() == title; });

  if (it != this->curves.end()) {
    // Curve already exists. Return here.
    return false;
  } else {
    // Create the curve.
    this->curveData.insert(title, new QPolygonF());
    QwtPlotCurve *newCurve = new QwtPlotCurve(title);
    newCurve->setPen(pen);
    newCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    newCurve->setSamples(*this->curveData[title]);
    newCurve->setAxes(QwtAxis::XBottom, QwtAxis::YRight);
    newCurve->attach(this);

    this->curves.append(newCurve);

    return true;
  }
}

bool LinePlot::removeCurve(const QString &title) {

  size_t oldSize = this->curves.size();

  auto it = std::remove_if(
      this->curves.begin(), this->curves.end(),
      [title](QwtPlotCurve *curve) { return curve->title() == title; });

  if (oldSize > this->curves.size()) {
    return true;
  } else {
    return false;
  }
}

void LinePlot::resetData() {}

QStringList LinePlot::getCurveTitles() const {
  QStringList retVal;

  for (auto &curve : this->curves) {
    retVal << curve->title().text();
  }

  return retVal;
}

} // namespace Gui
